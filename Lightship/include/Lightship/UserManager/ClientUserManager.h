#pragma once

#include "Lightship/UserManager/UserManager.h"

class ClientUserManager : public UserManager
{
public:
    ClientUserManager(Urho3D::Context* context);

private:
    void HandleNetworkMessage(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
};
