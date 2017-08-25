#include "lightship/Network/Protocol.h"
#include "lightship/Network/ChatProtocol.h"
#include "lightship/Network/UserManager.h"
#include "lightship/ClientAPI/ChatAPI.h"
#include <Urho3D/Core/Context.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/IO/MemoryBuffer.h>
#include <Urho3D/Network/Connection.h>
#include <Urho3D/Network/Network.h>
#include <Urho3D/Network/NetworkEvents.h>

using namespace Urho3D;

// ----------------------------------------------------------------------------
ChatProtocol::ChatProtocol(Context* context, ChatAPI* client) :
    Object(context),
    client_(client)
{
    if (client != NULL)
        SubscribeToEvent(E_NETWORKMESSAGE, URHO3D_HANDLER(ChatProtocol, HandleNetworkMessage_Client));
    else
        SubscribeToEvent(E_NETWORKMESSAGE, URHO3D_HANDLER(ChatProtocol, HandleNetworkMessage_Server));
}

// ----------------------------------------------------------------------------
void ChatProtocol::SendMessage(const Urho3D::String& message)
{
    Connection* serverConnection = GetSubsystem<Network>()->GetServerConnection();
    if (serverConnection == NULL)
    {
        URHO3D_LOGERROR("Cannot send chat message: No server connection");
        return;
    }

    VectorBuffer buffer;
    buffer.WriteUByte(GLOBAL);
    buffer.WriteString(message);
    serverConnection->SendMessage(MSG_CHATMESSAGE, true, false, buffer);
}

// ----------------------------------------------------------------------------
void ChatProtocol::HandleNetworkMessage_Server(StringHash eventType, VariantMap& eventData)
{
    using namespace NetworkMessage;
    int messageID = eventData[P_MESSAGEID].GetInt();
    Connection* connection = static_cast<Connection*>(eventData[P_CONNECTION].GetPtr());

    if (messageID == MSG_CHATMESSAGE)
    {
        MemoryBuffer buffer(eventData[P_DATA].GetBuffer());
        unsigned char messageTarget = buffer.ReadUByte();
        String message = buffer.ReadString();
        URHO3D_LOGDEBUGF("Received message \"%s\"", message.CString());

        String username = GetSubsystem<UserManager>()->GetUsername(connection);

        // Append username to it, then broadcast
        message = "<" + username + "> " + message;

        if (messageTarget & GLOBAL)
        {
            VectorBuffer outBuffer;
            outBuffer.WriteUByte(messageTarget);
            outBuffer.WriteString(message);
            GetSubsystem<Network>()->BroadcastMessage(MSG_CHATMESSAGE, true, false, outBuffer);
        }
    }
}

// ----------------------------------------------------------------------------
void ChatProtocol::HandleNetworkMessage_Client(StringHash eventType, VariantMap& eventData)
{
    using namespace NetworkMessage;
    int messageID = eventData[P_MESSAGEID].GetInt();

    if (messageID == MSG_CHATMESSAGE)
    {
        MemoryBuffer buffer(eventData[P_DATA].GetBuffer());
        unsigned char messageType = buffer.ReadUByte();
        String message = buffer.ReadString();

        URHO3D_LOGDEBUGF("Received message \"%s\"", message.CString());

        if (messageType & GLOBAL)
        {
            client_->AddMessage(message);
        }
    } /*
    else if (messageID == MSG_CONNECTEDUSERSLIST)
    {
        MemoryBuffer buffer(eventData[P_DATA].GetBuffer());
        NetworkMessageAction action = static_cast<NetworkMessageAction>(buffer.ReadUByte());
        switch (action)
        {
            case REQUEST_CONNECTED_USERS:
                URHO3D_LOGERRORF("Unexpected connected users request from server");
                break;

            case RECEIVE_CONNECTED_USERS:
            {
                connectedUsers_ = buffer.ReadStringVector();

                VariantMap& eventData = GetEventDataMap();
                eventData[ChatUserListChanged::P_USERLIST] = connectedUsers_;
                SendEvent(E_CHATUSERLISTCHANGED, eventData);
            }   break;

            case RECEIVE_JOINED_USER:
            {
                String username = buffer.ReadString();
                connectedUsers_.Push(username);

                AddChatMessage("--> " + username + " joined the server!");

                VariantMap& eventData = GetEventDataMap();
                eventData[ChatUserJoined::P_USERNAME] = username;
                SendEvent(E_CHATUSERJOINED, eventData);
            }   break;

            case RECEIVE_LEFT_USER:
            {
                String username = buffer.ReadString();
                connectedUsers_.Remove(username);

                AddChatMessage("<-- " + username + " left the server!");

                VariantMap& eventData = GetEventDataMap();
                eventData[ChatUserLeft::P_USERNAME] = username;
                SendEvent(E_CHATUSERLEFT, eventData);
            }   break;
        }
    }*/
}
