#include "lightship/ChatServerProtocol.h"
#include "lightship/Network/Protocol.h"
#include <Urho3D/Network/Network.h>
#include <Urho3D/Network/NetworkEvents.h>
#include <Urho3D/IO/VectorBuffer.h>
#include <Urho3D/IO/MemoryBuffer.h>
#include <Urho3D/IO/Log.h>

using namespace Urho3D;

// ----------------------------------------------------------------------------
ChatServerProtocol::ChatServerProtocol(Context* context, Scope scope) :
    ChatModel(context, scope)
{
    SubscribeToEvent(E_NETWORKMESSAGE, URHO3D_HANDLER(ChatServerProtocol, HandleNetworkMessage));
}

// ----------------------------------------------------------------------------
void ChatServerProtocol::AddMessage(const Urho3D::String& message)
{
    VectorBuffer buffer;
    unsigned char meta = ((GetScope() & 0x0F) << 0) |
                         ((CHAT_SEND_MESSAGE & 0x0F) << 4);
    buffer.WriteUByte(meta);
    buffer.WriteString(message);
    GetSubsystem<Network>()->BroadcastMessage(MSG_CHAT, true, false, buffer);
}

// ----------------------------------------------------------------------------
void ChatServerProtocol::ClearMessages()
{
    ChatModel::ClearMessages();
}

// ----------------------------------------------------------------------------
void ChatServerProtocol::HandleNetworkMessage(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
    using namespace NetworkMessage;
    int messageID = eventData[P_MESSAGEID].GetInt();
    Connection* connection = static_cast<Connection*>(eventData[P_CONNECTION].GetPtr());

    if (messageID != MSG_CHAT)
        return;

    MemoryBuffer buffer(eventData[P_DATA].GetBuffer());
    unsigned char meta = buffer.ReadUByte();
    Scope scope = static_cast<Scope>(meta & 0x0F);
    ChatNetwork action = static_cast<ChatNetwork>(meta >> 4);

    if (scope != GetScope())
        return;

    switch (action)
    {
        case CHAT_REQUEST_CONNECTED_USERS_LIST:
            break;

        case CHAT_SEND_MESSAGE:
        {
            String message = buffer.ReadString();
            URHO3D_LOGDEBUGF("Received message \"%s\"", message.CString());

            // Append username to it, then broadcast
            //String username = GetSubsystem<UserManager>()->GetUsername(connection);
            //message = "<" + username + "> " + message;
            AddMessage(message);
        } break;

        default:
            URHO3D_LOGERRORF("Received invalid action in chat server protocol: %d", action);
            break;
    }
}
