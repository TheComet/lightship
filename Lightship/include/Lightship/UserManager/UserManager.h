#pragma once

#include <Urho3D/Core/Object.h>

namespace Urho3D {
    class Connection;
}

class User;

class UserManager : public Urho3D::Object
{
    URHO3D_OBJECT(UserManager, Urho3D::Object)

public:
    typedef Urho3D::HashMap<int, Urho3D::SharedPtr<User>> Users;

    UserManager(Urho3D::Context* context);

    /*!
     * Creates a new user and returns it. The user will have a unique ID
     * generated and the username will be set.
     */
    virtual User* AddUser(const Urho3D::String& username);

    /*!
     * Removes a user, he will no longer be registered.
     */
    virtual bool RemoveUserByUID(int uid);
    virtual bool RemoveUserByConnection(Urho3D::Connection* connection);

    User* GetUserByUID(int uid) const;

    /*!
     * Looks for a user that has been associated with the specified connection.
     * This will only work if User::SetConnection() was previously called.
     */
    User* GetUserByConnection(Urho3D::Connection* connection) const;

    /// Returns a list of all user names.
    Urho3D::StringVector GetUsernames() const;

    /// Returns the number of users that are registered.
    int GetUserCount() const;

protected:
    Users users_;
};
