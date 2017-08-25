#include "lightship/Network/ClientProtocol.h"
#include "lightship/Map.h"
#include "lightship/MapState.h"
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Network/Network.h>
#include <Urho3D/Scene/Scene.h>

using namespace Urho3D;

// ----------------------------------------------------------------------------
ClientProtocol::ClientProtocol(Context* context, ClientAPI* client) :
    Object(context),
    client_(client)
{
}

// ----------------------------------------------------------------------------
bool ClientProtocol::ConnectToServer(const String& username, const String& address, unsigned int port)
{
    scene_ = new Scene(context_);

    scene_->CreateComponent<Octree>(LOCAL);
    MapState* mapState = scene_->CreateComponent<MapState>(LOCAL);
    Map* map = scene_->CreateComponent<Map>(LOCAL);
    map->SetState(mapState);

    VariantMap identity;
    identity["Username"] = username;
    if (GetSubsystem<Network>()->Connect(address, port, scene_, identity) == false)
    {
        scene_ = NULL;
        return false;
    }

    return true;
}

// ----------------------------------------------------------------------------
void ClientProtocol::AbortConnectingToServer()
{
}

// ----------------------------------------------------------------------------
void ClientProtocol::DisconnectFromServer()
{
    Network* network = GetSubsystem<Network>();
    if (network->GetServerConnection() == NULL)
        return;

    network->Disconnect();
    scene_ = NULL;
}

// ----------------------------------------------------------------------------
void ClientProtocol::CreateGame(const String& gameName)
{
}

// ----------------------------------------------------------------------------
void ClientProtocol::JoinGame(unsigned int gameID)
{
}

// ----------------------------------------------------------------------------
void ClientProtocol::LeaveGame()
{
}
