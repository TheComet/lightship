#pragma once

#include "Lightship/config.hpp"
#include "Lightship/UserManager/UserManager.hpp"

#include <Urho3D/UI/UIElement.h>

namespace Urho3D {
    class ListView;
}

namespace LS {

class LIGHTSHIP_PUBLIC_API ClientUserManager : public UserManager
{
    URHO3D_OBJECT(ClientUserManager, Urho3D::UIElement)

public:
    ClientUserManager(Urho3D::Context* context);
    static void RegisterObject(Urho3D::Context* context);

    User* AddUser(const Urho3D::String& username) override;
    bool RemoveUserByUID(int uid) override;

private:
    void HandleNetworkMessage(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

    Urho3D::ListView* usersList_;
};

}
