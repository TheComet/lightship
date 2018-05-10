#include "Lightship/UserManager/ClientUserManager.h"
#include "Lightship/UserManager/User.h"
#include "Lightship/Network/Protocol.h"
#include <Urho3D/Network/NetworkEvents.h>
#include <Urho3D/IO/MemoryBuffer.h>
#include <Urho3D/IO/Log.h>

using namespace Urho3D;

// ----------------------------------------------------------------------------
ClientUserManager::ClientUserManager(Context* context) :
    UserManager(context)
{
    SubscribeToEvent(E_SERVERCONNECTED, URHO3D_HANDLER(ClientUserManager, HandleServerConnected));
    SubscribeToEvent(E_SERVERDISCONNECTED, URHO3D_HANDLER(ClientUserManager, HandleServerDisconnected));
    SubscribeToEvent(E_NETWORKMESSAGE, URHO3D_HANDLER(ClientUserManager, HandleNetworkMessage));
}

// ----------------------------------------------------------------------------
void ClientUserManager::HandleServerConnected(StringHash eventType, VariantMap& eventData)
{
}

// ----------------------------------------------------------------------------
void ClientUserManager::HandleServerDisconnected(StringHash eventType, VariantMap& eventData)
{
}

// ----------------------------------------------------------------------------
void ClientUserManager::HandleNetworkMessage(StringHash eventType, VariantMap& eventData)
{
    using namespace NetworkMessage;
    int messageID = eventData[P_MESSAGEID].GetInt();

    MemoryBuffer buffer(eventData[P_DATA].GetBuffer());

    switch (messageID)
    {
        case MSG_USER_ADDED :
        {
            User* user = AddUser("");
            user->Load(buffer);
        } break;

        case MSG_USER_REMOVED :
        {
            int uid = buffer.ReadInt();
            RemoveUserByUID(uid);
        } break;

        case MSG_USER_REQUEST_LIST :
        {
            users_.Clear();

            int userCount = buffer.ReadUShort();
            for (int i = 0; i < userCount; ++i)
            {
                User* user = AddUser("");
                user->Load(buffer);
            }
        } break;

        default:
            break;
    }

    URHO3D_LOGDEBUG("Users connected are:");
    for (const auto& user : users_)
    {
        URHO3D_LOGDEBUGF("  - %s#%d", user.second_->GetUsername().CString(), user.second_->GetUID());
    }
}
