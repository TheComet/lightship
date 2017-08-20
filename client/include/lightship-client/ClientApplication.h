#pragma once

#include "lightship/API/ClientAPI.h"
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

    bool ConnectToServer(const Urho3D::String& address, unsigned int port) override;
    void AbortConnectingToServer() override;
    void DisconnectFromServer() override;
    void Quit() override;

    Urho3D::String GetUsername() const override;
    void SetUsername(const Urho3D::String& username) override;
    unsigned int GetGlobalID() const override;
    void SetGlobalID(unsigned int ID) override;
    char GetLocalID() const override;
    void SetLocalID(char ID) override;

    void CreateGame(const Urho3D::String& gameName) override;
    void JoinGame(unsigned int gameID) override;
    void LeaveGame() override;

    void SendGlobalChatMessage(const Urho3D::String& message) override;
    void SendLobbyChatMessage(const Urho3D::String& message) override;
    void SendInGameChatMessage(const Urho3D::String& message) override;

private:
    void RegisterStuff();
    void SubscribeToEvents();
    void CreateDebugHud();
    void CreateCamera();

    void HandleKeyDown(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandlePostRenderUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleChatMessage(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

private:
    DebugDrawMode debugDrawMode_;
    Urho3D::SharedPtr<TrackingCamera> trackingCamera_;
    Urho3D::SharedPtr<Urho3D::Scene> scene_;
    Urho3D::SharedPtr<Urho3D::DebugHud> debugHud_;
    Urho3D::String username_;
    unsigned int guid_;
    char luid_;
};
