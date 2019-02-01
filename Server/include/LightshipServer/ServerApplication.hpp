#pragma once

#include <Urho3D/Engine/Application.h>
#include <Urho3D/Container/HashMap.h>

namespace Urho3D {
    class Connection;
    class DebugHud;
    class Scene;
    class Text;
    class XMLFile;
}

namespace LS {

class MapState;

class ServerApplication : public Urho3D::Application
{
public:
    ServerApplication(Urho3D::Context* context);

    virtual void Setup() override;
    virtual void Start() override;
    virtual void Stop() override;

private:
    bool ProcessCommandLine();
    void RegisterStuff();
    void LoadMap(const Urho3D::String& fileName);
    void CreatePlayer();
    void SubscribeToEvents();

    void HandleFileChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleExitRequested(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleClientConnected(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleClientDisconnected(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleClientIdentity(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

    Urho3D::SharedPtr<MapState> map_;
    Urho3D::SharedPtr<Urho3D::Scene> scene_;
    Urho3D::SharedPtr<Urho3D::XMLFile> xmlScene_;
};

}
