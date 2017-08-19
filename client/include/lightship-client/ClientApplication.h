#pragma once

#include "lightship-client/ClientAPI.h"
#include <Urho3D/Engine/Application.h>

class TrackingCamera;

namespace Urho3D {
    class DebugHud;
    class Scene;
    class XMLFile;
}

class ClientApplication : public Urho3D::Application, public ClientAPI
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

    void ConnectToServer(const Urho3D::String& address, unsigned int port) override;
    void DisconnectFromServer() override;

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
    Urho3D::SharedPtr<Urho3D::Scene> scene_;
    Urho3D::SharedPtr<Urho3D::DebugHud> debugHud_;
};
