#pragma once

#include <Urho3D/Core/Object.h>

class ClientAPI;
namespace Urho3D {
    class Scene;
}

class ClientProtocol : public Urho3D::Object
{
    URHO3D_OBJECT(ClientProtocol, Urho3D::Object)

public:
    ClientProtocol(Urho3D::Context* context, ClientAPI* client);

    bool ConnectToServer(const Urho3D::String& username, const Urho3D::String& address, unsigned int port);
    void AbortConnectingToServer();
    void DisconnectFromServer();

    void CreateGame(const Urho3D::String& gameName);
    void JoinGame(unsigned int gameID);
    void LeaveGame();

private:
    Urho3D::SharedPtr<Urho3D::Scene> scene_;
    ClientAPI* client_;
};
