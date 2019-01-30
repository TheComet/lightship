#pragma once

#include "Lightship/config.hpp"
#include <Urho3D/Scene/Component.h>

namespace LS {

class LIGHTSHIP_PUBLIC_API Player : public Urho3D::Component
{
    URHO3D_OBJECT(Player, Urho3D::Component);

public:
    Player(Urho3D::Context* context);
    static void RegisterObject(Urho3D::Context* context);

    void SetPrimaryColor(const Urho3D::Color& color);
    const Urho3D::Color& GetPrimaryColor() const;

    /*
    void SetDecorativeColor(const Urho3D::Color& color);
    const Urho3D::Color& GetDecorativeColor() const;

    void SetPosition(const Urho3D::Vector2& position);
    const Urho3D::Vector2& GetPosition() const;

    void SetSpeed(float speed);
    float GetSpeed() const;*/

protected:
    virtual void OnNodeSet(Urho3D::Node* node) override;

private:
    void HandleUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

private:
    Urho3D::SharedPtr<Urho3D::Node> modelNode_;
    Urho3D::Color playerColor_;
};

}
