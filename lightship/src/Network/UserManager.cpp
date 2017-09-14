#include "lightship/Network/UserManager.h"
#include "lightship/Network/UserManagerEvents.h"
#include <Urho3D/IO/MemoryBuffer.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Network/Network.h>
#include <Urho3D/Network/NetworkEvents.h>
#include <Urho3D/Network/Connection.h>

using namespace Urho3D;

// ----------------------------------------------------------------------------
UserManager::UserManager(Context* context) :
    Object(context)
{
    SubscribeToEvent(E_CLIENTCONNECTED, URHO3D_HANDLER(UserManager, HandleClientConnected));
    SubscribeToEvent(E_CLIENTIDENTITY, URHO3D_HANDLER(UserManager, HandleClientIdentity));
    SubscribeToEvent(E_CLIENTDISCONNECTED, URHO3D_HANDLER(UserManager, HandleClientDisconnected));
    SubscribeToEvent(E_NETWORKMESSAGE, URHO3D_HANDLER(UserManager, HandleNetworkMessage));
}

// ----------------------------------------------------------------------------
String UserManager::GetUsername(Connection* connection) const
{
    const User* user = onlineUsers_[connection];
    if (user == NULL)
        return "";
    return user->name_;
}

// ----------------------------------------------------------------------------
void UserManager::SetUsername(Connection* connection, const String& username)
{
    onlineUsers_[connection] = {
        username,
        connection->GetAddress()
    };
}

// ----------------------------------------------------------------------------
void UserManager::HandleClientConnected(StringHash eventType, VariantMap& eventData)
{
}

// ----------------------------------------------------------------------------
void UserManager::HandleClientIdentity(StringHash eventType, VariantMap& eventData)
{
    using namespace ClientIdentity;

    Connection* connection = static_cast<Connection*>(eventData[P_CONNECTION].GetPtr());
    const VariantMap& identity = connection->GetIdentity();
    Variant* var = identity["Username"];
    if (var == NULL)
    {
        URHO3D_LOGERROR("Client didn't provide username!");
        connection->Disconnect();
        return;
    }

    String username = var->GetString();
    if (username.Empty())
    {
        URHO3D_LOGERROR("Client provided an empty username!");
        connection->Disconnect();
        return;
    }

    for (OnlineUserList::ConstIterator it = onlineUsers_.Begin(); it != onlineUsers_.End(); ++it)
    {
        if (it->second_.name_ == username)
        {
            eventData[P_ALLOW] = false;
            return;
        }
    }

    SetUsername(connection, username);

    VariantMap& data = GetEventDataMap();
    data[UserJoined::P_USERNAME] = username;
    SendEvent(E_USERJOINED, data);
}

// ----------------------------------------------------------------------------
void UserManager::HandleClientDisconnected(StringHash eventType, VariantMap& eventData)
{
    using namespace ClientDisconnected;

    Connection* connection = static_cast<Connection*>(eventData[P_CONNECTION].GetPtr());
    OnlineUserList::Iterator it = onlineUsers_.Find(connection);
    if (it == onlineUsers_.End())
        return;

    VariantMap& data = GetEventDataMap();
    data[UserLeft::P_USERNAME] = it->second_.name_;
    SendEvent(E_USERJOINED, data);

    offlineUsers_.Push(it->second_);
    onlineUsers_.Erase(it);
}

// ----------------------------------------------------------------------------
void UserManager::HandleNetworkMessage(StringHash eventType, VariantMap& eventData)
{/*
    using namespace NetworkMessage;
    int messageID = eventData[P_MESSAGEID].GetInt();

    if (messageID != MSG_USERMANAGER)
        return;

    MemoryBuffer buffer(eventData[P_DATA].GetBuffer());
    Chat::NetworkMessageAction action = static_cast<Chat::NetworkMessageAction>(buffer.ReadUByte());
    switch (action)
    {
        case Chat::REQUEST_CONNECTED_USERS:
        {
            URHO3D_LOGDEBUG("User list requested");

            StringVector users;
            for (OnlineUserList::ConstIterator it = onlineUsers_.Begin(); it != onlineUsers_.End(); ++it)
                users.Push(it->second_.name_);
            VectorBuffer buffer;
            buffer.WriteUByte(Chat::RECEIVE_CONNECTED_USERS);
            buffer.WriteStringVector(users);

            Connection* connection = static_cast<Connection*>(eventData[P_CONNECTION].GetPtr());
            connection->SendMessage(MSG_USERMANAGER, true, false, buffer);
        } break;

        case Chat::RECEIVE_CONNECTED_USERS:
        case Chat::RECEIVE_JOINED_USER:
        case Chat::RECEIVE_LEFT_USER:
            URHO3D_LOGERROR("Unexpected client action in MSG_USERMANAGER");
            break;
    }*/
}
