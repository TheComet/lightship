#include "Lightship/Chat/ChatServer.h"
#include "Lightship/Network/Protocol.h"
#include <Urho3D/Network/Network.h>
#include <Urho3D/Network/NetworkEvents.h>
#include <Urho3D/IO/VectorBuffer.h>
#include <Urho3D/IO/MemoryBuffer.h>
#include <Urho3D/IO/Log.h>

using namespace Urho3D;

// ----------------------------------------------------------------------------
ChatServer::ChatServer(Context* context) :
    Object(context),
    insertIndex_(0)
{
    SubscribeToEvent(E_NETWORKMESSAGE, URHO3D_HANDLER(ChatServer, HandleNetworkMessage));
}

// ----------------------------------------------------------------------------
Urho3D::StringVector ChatServer::GetMessages() const
{
    Urho3D::StringVector result;

    // NOTE: Using Size() here instead of limit_, as there may be less messages
    // in the buffer than limit_.
    if (messages_.Size() == 0)
        return result;
    int iter = insertIndex_;  // This is where the next message would be inserted, i.e. the oldest message
    if (iter > messages_.Size() - 1)
        iter = 0;

    do
    {
        result.Push(messages_[iter]);
        if (++iter > messages_.Size() - 1)
            iter = 0;
    }
    while (iter != insertIndex_);

    return result;
}

// ----------------------------------------------------------------------------
Urho3D::String ChatServer::GetNewestMessage() const
{
    int newestIdx = insertIndex_ - 1;
    if (newestIdx < 0)
        newestIdx = messages_.Size() - 1;
    if (newestIdx < 0)
        return "";

    return messages_[newestIdx];
}

// ----------------------------------------------------------------------------
void ChatServer::AddMessage(const String& message, const Color& color)
{
    if (messages_.Size() < GetMessageLimit())
    {
        messages_.Push(message);
        Advance();
    }
    else
    {
        messages_[Advance()] = message;
    }
}

// ----------------------------------------------------------------------------
void ChatServer::ClearMessages()
{
    messages_.Clear();
    insertIndex_ = 0;
}

// ----------------------------------------------------------------------------
int ChatServer::Advance()
{
    int insert = insertIndex_++;
    if (insertIndex_ >= GetMessageLimit())
        insertIndex_ = 0;
    return insert;
}

// ----------------------------------------------------------------------------
void ChatServer::HandleNetworkMessage(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
    using namespace NetworkMessage;
    int messageID = eventData[P_MESSAGEID].GetInt();
    Connection* connection = static_cast<Connection*>(eventData[P_CONNECTION].GetPtr());

    // Normal chat messages are stored and broadcasted to everyone
    if (messageID == MSG_CHAT_MESSAGE)
    {
        MemoryBuffer recBuf(eventData[P_DATA].GetBuffer());
        String chatMessage = recBuf.ReadString();
        URHO3D_LOGDEBUGF("Received message: %s", chatMessage.CString());
        AddMessage(chatMessage, Color::WHITE);

        // Append username to it, then broadcast
        //String username = GetSubsystem<UserManager>()->GetUsername(connection);
        //message = "<" + username + "> " + message;

        VectorBuffer sendBuf;
        sendBuf.WriteString(chatMessage);
        GetSubsystem<Network>()->BroadcastMessage(MSG_CHAT_MESSAGE, true, false, sendBuf);
    }

    // If a user requests message history, send the entire buffer
    if (messageID == MSG_CHAT_REQUEST_HISTORY)
    {
        StringVector allMessages = GetMessages();

        URHO3D_LOGDEBUGF("Request for message history, sending %d messages", allMessages.Size());

        VectorBuffer sendBuf;
        sendBuf.WriteInt(allMessages.Size());
        for (int i = 0; i < allMessages.Size(); ++i)
        {
            sendBuf.WriteString(allMessages[i]);
        }

        connection->SendMessage(MSG_CHAT_REQUEST_HISTORY, true, false, sendBuf);
    }
}
