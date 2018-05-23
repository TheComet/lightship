#include "Lightship/UserManager/ServerUserManager.h"
#include "Lightship/UserManager/User.h"
#include "Lightship/UserManager/Events.h"
#include "Lightship/Network/Protocol.h"
#include <Urho3D/IO/Log.h>
#include <Urho3D/IO/MemoryBuffer.h>
#include <Urho3D/Network/Connection.h>
#include <Urho3D/Network/NetworkEvents.h>
#include <Urho3D/Network/Network.h>

using namespace Urho3D;

// ----------------------------------------------------------------------------
ServerUserManager::ServerUserManager(Urho3D::Context* context) :
    UserManager(context),
    maxUsernameLength_(32)
{
    SubscribeToEvent(E_CLIENTCONNECTED, URHO3D_HANDLER(ServerUserManager, HandleClientConnected));
    SubscribeToEvent(E_CLIENTDISCONNECTED, URHO3D_HANDLER(ServerUserManager, HandleClientDisconnected));
    SubscribeToEvent(E_CLIENTIDENTITY, URHO3D_HANDLER(ServerUserManager, HandleClientIdentity));
    SubscribeToEvent(E_NETWORKMESSAGE, URHO3D_HANDLER(ServerUserManager, HandleNetworkMessage));
}

// ----------------------------------------------------------------------------
User* ServerUserManager::AddUser(const String& username)
{
    User* user = UserManager::AddUser(username);

    VectorBuffer buffer;
    user->Save(buffer);
    GetSubsystem<Network>()->BroadcastMessage(MSG_USER_ADDED, true, true, buffer);

    return user;
}

// ----------------------------------------------------------------------------
bool ServerUserManager::RemoveUserByUID(int uid)
{
    if (UserManager::RemoveUserByUID(uid) == false)
        return false;

    VectorBuffer buffer;
    buffer.WriteInt(uid);
    GetSubsystem<Network>()->BroadcastMessage(MSG_USER_REMOVED, true, true, buffer);
    return true;
}

// ----------------------------------------------------------------------------
void ServerUserManager::SendUserList(Connection* recipient)
{
    VectorBuffer buffer;
    buffer.WriteUShort(GetUserCount());
    for (const auto& user : users_)
    {
        user.second_->Save(buffer);
    }
    recipient->SendMessage(MSG_USER_REQUEST_LIST, true, true, buffer);
}

// ----------------------------------------------------------------------------
void ServerUserManager::HandleClientIdentity(StringHash eventType, VariantMap& eventData)
{
    using namespace ClientIdentity;

    Connection* connection = static_cast<Connection*>(eventData[P_CONNECTION].GetPtr());
    VariantMap& identity = connection->GetIdentity();

    String username = identity["Username"].GetString();
    if (username.Empty())
    {
        URHO3D_LOGERROR("Client provided an empty username!");
        eventData[P_ALLOW] = false;
        return;
    }
    if (username.Length() > maxUsernameLength_)
    {
        URHO3D_LOGERRORF("Client provided a username longer than %d: %s", maxUsernameLength_, username.CString());
        eventData[P_ALLOW] = false;
        return;
    }

    /*
     * Username seems ok, send the existing userlist first (excluding this new
     * user) before creating the user so all clients definitely have the same
     * list.
     */
    SendUserList(connection);
    User* user = AddUser(username);

    // Associate connection with user for easier lookups later
    user->SetConnection(connection);

    /*
     * With the user verified, notify everyone server-side that the client
     * has successfully connected. Note that we can re-use the event data
     * since it's the same.
     */
    SendEvent(E_CLIENTCONNECTEDANDVERIFIED, eventData);

    // The client should also be notified that their connection was accepted
    GetSubsystem<Network>()->RegisterRemoteEvent(E_SERVERCONNECTEDANDVERIFIED);
    connection->SendRemoteEvent(E_SERVERCONNECTEDANDVERIFIED, false);
}

// ----------------------------------------------------------------------------
void ServerUserManager::HandleClientConnected(StringHash eventType, VariantMap& eventData)
{
}

// ----------------------------------------------------------------------------
void ServerUserManager::HandleClientDisconnected(StringHash eventType, VariantMap& eventData)
{
    using namespace ClientDisconnected;

    Connection* connection = static_cast<Connection*>(eventData[P_CONNECTION].GetPtr());
    RemoveUserByConnection(connection);
}

// ----------------------------------------------------------------------------
void ServerUserManager::HandleNetworkMessage(StringHash eventType, VariantMap& eventData)
{
    using namespace NetworkMessage;
    int messageID = eventData[P_MESSAGEID].GetInt();

    if (messageID != MSG_USER_REQUEST_LIST)
        return;

    Connection* connection = static_cast<Connection*>(eventData[P_CONNECTION].GetPtr());
    URHO3D_LOGDEBUGF("Client %s requested user list", GetUserByUID(connection->GetIdentity()["UID"].GetInt())->GetUsername().CString());

    SendUserList(connection);
}
