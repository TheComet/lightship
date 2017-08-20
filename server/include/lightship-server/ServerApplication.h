#pragma once

#include "lightship/API/ServerAPI.h"
#include <Urho3D/Engine/Application.h>
#include <Urho3D/Container/HashMap.h>

class MapState;

namespace Urho3D {
    class Connection;
    class DebugHud;
    class Scene;
    class Text;
    class XMLFile;
}

struct ConnectedUser
{
    Urho3D::String name_;
};

class ServerApplication : public Urho3D::Application, public ServerAPI
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
    void HandleClientIdentity(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleNetworkMessage(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

    void HandleFileChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleExitRequested(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

    typedef Urho3D::HashMap<Urho3D::Connection*, ConnectedUser> ConnectedUsers;

    Urho3D::SharedPtr<MapState> map_;
    Urho3D::SharedPtr<Urho3D::Scene> scene_;
    Urho3D::SharedPtr<Urho3D::XMLFile> xmlScene_;
    ConnectedUsers connectedUsers_;
};
