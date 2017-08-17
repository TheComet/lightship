#pragma once

#include <Urho3D/Engine/Application.h>

class MapState;

namespace Urho3D {
    class DebugHud;
    class Scene;
    class Text;
    class XMLFile;
}

class ServerApplication : public Urho3D::Application
{
public:
    ServerApplication(Urho3D::Context* context);

    virtual void Setup() override;
    virtual void Start() override;
    virtual void Stop() override;

private:
    void RegisterStuff();
    void LoadMap(const Urho3D::String& fileName);
    void CreatePlayer();
    void SubscribeToEvents();

    void HandleConnectFailed(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleClientConnected(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleClientDisonnected(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

    void HandleFileChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleExitRequested(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

    Urho3D::SharedPtr<MapState> map_;
    Urho3D::SharedPtr<Urho3D::Scene> scene_;
    Urho3D::SharedPtr<Urho3D::XMLFile> xmlScene_;
};
