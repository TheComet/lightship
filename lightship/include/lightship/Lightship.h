#pragma once

#include "lightship/config.h"
#include <Urho3D/Scene/Component.h>

class TrackingCamera;

class LIGHTSHIP_PUBLIC_API Lightship : public Urho3D::Component
{
    URHO3D_OBJECT(Lightship, Urho3D::Component);

public:
    Lightship(Urho3D::Context* context);
    static void RegisterSubsystemsAndComponents(Urho3D::Context* context);

private:
    void LoadScene();
    void CreateCamera();
    void CreatePlayer();

private:
    Urho3D::SharedPtr<TrackingCamera> trackingCamera_;
};
