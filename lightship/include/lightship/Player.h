#pragma once

#include <Urho3D/Scene/Component.h>

class Player : public Urho3D::Component
{
    URHO3D_OBJECT(Player, Urho3D::Component);

public:
    Player(Urho3D::Context* context);
    static void RegisterObject(Urho3D::Context* context);

protected:
    virtual void OnNodeSet(Urho3D::Node* node) override;
};
