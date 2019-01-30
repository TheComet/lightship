#include "Lightship/UserManager/ClientUserManager.hpp"
#include "Lightship/UserManager/User.hpp"
#include "Lightship/Network/Protocol.hpp"

#include <Urho3D/Core/Context.h>
#include <Urho3D/Network/NetworkEvents.h>
#include <Urho3D/IO/MemoryBuffer.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/UI/ListView.h>
#include <Urho3D/UI/Text.h>

using namespace Urho3D;

namespace LS {

// ----------------------------------------------------------------------------
ClientUserManager::ClientUserManager(Context* context) :
    UserManager(context)
{
    SetLayoutMode(LM_HORIZONTAL);
    usersList_ = CreateChild<ListView>();

    SubscribeToEvent(E_NETWORKMESSAGE, URHO3D_HANDLER(ClientUserManager, HandleNetworkMessage));
}

// ----------------------------------------------------------------------------
void ClientUserManager::RegisterObject(Context* context)
{
    context->RegisterFactory<ClientUserManager>("Lightship");
}

// ----------------------------------------------------------------------------
User* ClientUserManager::AddUser(const String& username)
{
    Text* text = usersList_->CreateChild<Text>();
    text->SetText(username);
    return UserManager::AddUser(username);
}

// ----------------------------------------------------------------------------
bool ClientUserManager::RemoveUserByUID(int uid)
{
    return UserManager::RemoveUserByUID(uid);
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
            RemoveAllUsers();

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
}

}
