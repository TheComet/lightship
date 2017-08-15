#pragma once

#include <Urho3D/Engine/Application.h>

class MapState;

namespace Urho3D {
    class DebugHud;
    class Scene;
    class XMLFile;
}

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
    void CreateDebugHud();
    void SubscribeToEvents();

    void HandleKeyDown(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandlePostRenderUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

private:
    DebugDrawMode debugDrawMode_;
    Urho3D::SharedPtr<MapState> map_;
    Urho3D::SharedPtr<Urho3D::Scene> scene_;
    Urho3D::SharedPtr<Urho3D::XMLFile> xmlScene_;
    Urho3D::SharedPtr<Urho3D::DebugHud> debugHud_;
};
