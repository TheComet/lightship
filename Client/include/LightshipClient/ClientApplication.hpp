#pragma once

#include <Urho3D/Engine/Application.h>

namespace Urho3D {
    class DebugHud;
    class Scene;
    class XMLFile;
}

namespace LS {

class TrackingCamera;

class ClientApplication : public Urho3D::Application
{
public:
    enum DebugDrawMode
    {
        DRAW_NONE = 0,
        DRAW_PHYSICS
    };

    ClientApplication(Urho3D::Context* context);

    virtual void Setup() override;
    virtual void Start() override;
    virtual void Stop() override;

private:
    void RegisterStuff();
    void SubscribeToEvents();
    void CreateDebugHud();
    void CreateCamera();

    void HandleKeyDown(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandlePostRenderUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

private:
    DebugDrawMode debugDrawMode_;
    Urho3D::SharedPtr<TrackingCamera> trackingCamera_;
    Urho3D::SharedPtr<Urho3D::DebugHud> debugHud_;
    Urho3D::SharedPtr<Urho3D::Scene> scene_;
};

}
