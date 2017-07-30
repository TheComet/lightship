#pragma once

#include <Urho3D/Engine/Application.h>

namespace Urho3D {
    class DebugHud;
    class Node;
    class Scene;
    class Text;
    class XMLFile;
}

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
    void LoadScene(const Urho3D::String& fileName);
    void CreateCamera();
    void CreatePlayer();
    void CreateDebugHud();

    void HandleKeyDown(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandlePostRenderUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleFileChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

    DebugDrawMode debugDrawMode_;

    Urho3D::SharedPtr<Urho3D::Scene> scene_;
    Urho3D::SharedPtr<Urho3D::XMLFile> xmlScene_;
    Urho3D::SharedPtr<Urho3D::Node> playerNode_;
    Urho3D::SharedPtr<Urho3D::Node> cameraOffsetNode_;
    Urho3D::SharedPtr<Urho3D::Node> cameraRotateNode_;
    Urho3D::SharedPtr<Urho3D::DebugHud> debugHud_;
};
