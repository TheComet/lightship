#pragma once

#include "Lightship/UserManager/User.h"
#include <Urho3D/Core/RefCounted.h>

namespace Urho3D {
    class Connection;
}

class UserManagerModel : public Urho3D::RefCounted
{
public:
/*
    const User& ConnectionToUser(Urho3D::Connection* connection) const;
    void SetUsername(Urho3D::Connection* connection, const Urho3D::String& username);*/

private:
    /*
    typedef Urho3D::HashMap<Urho3D::Connection*, User> OnlineUserList;
    typedef Urho3D::Vector<User>                       OfflineUserList;

    OnlineUserList  onlineUsers_;
    OfflineUserList offlineUsers_;*/
};
