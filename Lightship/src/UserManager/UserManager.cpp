#include "Lightship/UserManager/UserManager.hpp"
#include "Lightship/UserManager/User.hpp"
#include <Urho3D/IO/Log.h>

using namespace Urho3D;

namespace LS {

static int g_uid = 0;

// ----------------------------------------------------------------------------
UserManager::UserManager(Context* context) :
    UIElement(context)
{
}

// ----------------------------------------------------------------------------
User* UserManager::AddUser(const String& username)
{
    URHO3D_LOGDEBUGF("Adding user %s (id: %d)", username.CString(), g_uid);
    User* user = new User(context_, username, g_uid++);
    users_[user->GetUID()] = user;
    return user;
}

// ----------------------------------------------------------------------------
bool UserManager::RemoveUserByUID(int uid)
{
    URHO3D_LOGDEBUGF("Removing user %d", uid);
    return users_.Erase(uid);
}

// ----------------------------------------------------------------------------
bool UserManager::RemoveUserByConnection(Connection* connection)
{
    User* user = GetUserByConnection(connection);
    if (user == nullptr)
    {
        URHO3D_LOGERROR("Failed to RemoveUserByConnection(), user with connection doesn't exist!");
        return false;
    }
    return RemoveUserByUID(user->GetUID());
}

// ----------------------------------------------------------------------------
void UserManager::RemoveAllUsers()
{
    users_.Clear();
}

// ----------------------------------------------------------------------------
User* UserManager::GetUserByUID(int uid) const
{
    return *users_[uid];
}

// ----------------------------------------------------------------------------
User* UserManager::GetUserByConnection(Connection* connection) const
{
    for (const auto& user : users_)
    {
        if (user.second_->GetConnection() == connection)
            return user.second_;
    }

    return nullptr;
}

// ----------------------------------------------------------------------------
StringVector UserManager::GetUsernames() const
{
    StringVector sv;
    for (const auto& user : users_)
        sv.Push(user.second_->GetUsername());
    return sv;
}

// ----------------------------------------------------------------------------
int UserManager::GetUserCount() const
{
    return users_.Size();
}

// ----------------------------------------------------------------------------
const UserManager::Users& UserManager::GetUsers() const
{
    return users_;
}

}
