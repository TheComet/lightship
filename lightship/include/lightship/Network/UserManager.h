#pragma once

#include <Urho3D/Core/Object.h>

namespace Urho3D {
    class Connection;
}

class UserManager : public Urho3D::Object
{
    URHO3D_OBJECT(UserManager, Urho3D::Object)

public:
    struct User
    {
        Urho3D::String name_;
        Urho3D::String address_;
    };

    UserManager(Urho3D::Context* context);

    Urho3D::String GetUsername(Urho3D::Connection* connection) const;
    void SetUsername(Urho3D::Connection* connection, const Urho3D::String& username);

private:
    void HandleClientConnected(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleClientIdentity(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleClientDisconnected(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleNetworkMessage(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

private:
    typedef Urho3D::HashMap<Urho3D::Connection*, User> OnlineUserList;
    typedef Urho3D::Vector<User>                       OfflineUserList;

    OnlineUserList  onlineUsers_;
    OfflineUserList offlineUsers_;
};
