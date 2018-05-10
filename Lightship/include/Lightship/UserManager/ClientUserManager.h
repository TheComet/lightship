#pragma once

#include "Lightship/UserManager/UserManager.h"

class ClientUserManager : public UserManager
{
public:
    ClientUserManager(Urho3D::Context* context);

private:
    void HandleServerConnected(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleServerDisconnected(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleNetworkMessage(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
};
