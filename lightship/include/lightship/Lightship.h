#pragma once

#include <Urho3D/Engine/Application.h>

namespace Urho3D {
    class DebugHud;
    class Node;
    class Scene;
    class Text;
    class XMLFile;
}

class Map;
class TrackingCamera;

class Lightship : public Urho3D::Application
{
public:
    enum DebugDrawMode
    {
        DRAW_NONE = 0,
        DRAW_PHYSICS
    };

    Lightship(Urho3D::Context* context);

    virtual void Setup() override;
    virtual void Start() override;
    virtual void Stop() override;

private:
    void RegisterSubsystems();
    void RegisterComponents();
    void LoadScene();
    void CreateCamera();
    void CreatePlayer();
    void CreateDebugHud();

    void HandleKeyDown(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandlePostRenderUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleFileChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

    DebugDrawMode debugDrawMode_;

    Urho3D::SharedPtr<TrackingCamera> trackingCamera_;
    Urho3D::SharedPtr<Map> map_;
    Urho3D::SharedPtr<Urho3D::Scene> scene_;
    Urho3D::SharedPtr<Urho3D::XMLFile> xmlScene_;
    Urho3D::SharedPtr<Urho3D::DebugHud> debugHud_;
};
