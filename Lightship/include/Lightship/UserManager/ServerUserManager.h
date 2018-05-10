#pragma once

#include "Lightship/UserManager/UserManager.h"
#include <Urho3D/Core/StringUtils.h>

class ServerUserManager : public UserManager
{
public:
    ServerUserManager(Urho3D::Context* context);

    virtual User* AddUser(const Urho3D::String& username) override;
    virtual bool RemoveUserByUID(int uid) override;

private:
    void SendUserList(Urho3D::Connection* recipient);

    void HandleClientConnected(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleClientIdentity(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleClientDisconnected(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleNetworkMessage(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

private:
    int maxUsernameLength_;
};
