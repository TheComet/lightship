#include "lightship/ChatClientProtocol.h"
#include "lightship/Network/Protocol.h"
#include <Urho3D/Network/Network.h>
#include <Urho3D/Network/NetworkEvents.h>
#include <Urho3D/IO/VectorBuffer.h>
#include <Urho3D/IO/MemoryBuffer.h>
#include <Urho3D/IO/Log.h>

using namespace Urho3D;

// ----------------------------------------------------------------------------
ChatClientProtocol::ChatClientProtocol(Context* context, Scope scope) :
    ChatModel(context, scope)
{
    SubscribeToEvent(E_NETWORKMESSAGE, URHO3D_HANDLER(ChatClientProtocol, HandleNetworkMessage));
}

// ----------------------------------------------------------------------------
void ChatClientProtocol::RequestConnectedUsersList()
{
    VectorBuffer buffer;
    Connection* serverConnection = GetSubsystem<Network>()->GetServerConnection();
    if (serverConnection == NULL)
        return;

    unsigned char meta = ((GetScope() & 0x0F) << 0) |
                         ((CHAT_REQUEST_CONNECTED_USERS_LIST & 0x0F) << 4);
    buffer.WriteUByte(meta);
    serverConnection->SendMessage(MSG_CHAT, true, false, buffer);
}

// ----------------------------------------------------------------------------
void ChatClientProtocol::AddMessage(const Urho3D::String& message)
{
    VectorBuffer buffer;
    Connection* serverConnection = GetSubsystem<Network>()->GetServerConnection();
    if (serverConnection == NULL)
        return;

    unsigned char meta = ((GetScope() & 0x0F) << 0) |
                         ((CHAT_REQUEST_CONNECTED_USERS_LIST & 0x0F) << 4);
    buffer.WriteUByte(meta);
    buffer.WriteString(message);
    serverConnection->SendMessage(MSG_CHAT, true, false, buffer);
}

// ----------------------------------------------------------------------------
void ChatClientProtocol::ClearMessages()
{
    ChatModel::ClearMessages();
}

// ----------------------------------------------------------------------------
void ChatClientProtocol::HandleNetworkMessage(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
    using namespace NetworkMessage;
    int messageID = eventData[P_MESSAGEID].GetInt();

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
        case CHAT_SEND_MESSAGE:
        {
            String message = buffer.ReadString();
            URHO3D_LOGDEBUGF("Received message \"%s\"", message.CString());
            AddMessage(message);
        } break;

        default:
            URHO3D_LOGERRORF("Received invalid action in chat client protocol: %d", action);
            break;
    }
}
