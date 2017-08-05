#pragma once

#include "lightship/config.h"
#include <Urho3D/Scene/Component.h>

class LIGHTSHIP_PUBLIC_API Player : public Urho3D::Component
{
    URHO3D_OBJECT(Player, Urho3D::Component);

public:
    Player(Urho3D::Context* context);
    static void RegisterObject(Urho3D::Context* context);

protected:
    virtual void OnNodeSet(Urho3D::Node* node) override;

private:
    void HandleUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

private:
    Urho3D::SharedPtr<Urho3D::Node> modelNode_;
};
