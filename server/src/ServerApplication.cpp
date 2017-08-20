#include "lightship/Chat.h"
#include "lightship/GameConfig.h"
#include "lightship/Map.h"
#include "lightship/MapState.h"
#include "lightship/Player.h"
#include "lightship/Protocol.h"
#include "lightship-server/ServerApplication.h"
#include "lightship-server/SignalHandler.h"
#include <Urho3D/AngelScript/Script.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Light.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/IO/MemoryBuffer.h>
#include <Urho3D/Resource/ResourceEvents.h>
#include <Urho3D/Network/Network.h>
#include <Urho3D/Network/NetworkEvents.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Scene/Scene.h>

#include <iostream>

using namespace Urho3D;

// ----------------------------------------------------------------------------
ServerApplication::ServerApplication(Context* context) :
    Application(context)
{
}

// ----------------------------------------------------------------------------
void ServerApplication::Setup()
{
    engineParameters_["Headless"] = true;
}

// ----------------------------------------------------------------------------
void ServerApplication::Start()
{
    // configure resource cache to auto-reload resources when they change
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    cache->SetAutoReloadResources(true);

    RegisterStuff();
    SubscribeToEvents();

    GetSubsystem<Log>()->SetLevel(LOG_DEBUG);

    GetSubsystem<Network>()->StartServer(1337);
    LoadMap("Maps/(10) Adam.xml");
}

// ----------------------------------------------------------------------------
void ServerApplication::Stop()
{
    GetSubsystem<Network>()->StopServer();
}

// ----------------------------------------------------------------------------
void ServerApplication::RegisterStuff()
{
    // Server only subsystems
    context_->RegisterSubsystem(new SignalHandler(context_));

    // Client/Server subsystems
    context_->RegisterSubsystem(new Script(context_));
    context_->RegisterSubsystem(new GameConfig(context_));

    // Client/Server components
    Map::RegisterObject(context_);
    MapState::RegisterObject(context_);
    Player::RegisterObject(context_);
}

// ----------------------------------------------------------------------------
void ServerApplication::SubscribeToEvents()
{
    SubscribeToEvent(E_CONNECTFAILED, URHO3D_HANDLER(ServerApplication, HandleConnectFailed));
    SubscribeToEvent(E_CLIENTCONNECTED, URHO3D_HANDLER(ServerApplication, HandleClientConnected));
    SubscribeToEvent(E_CLIENTDISCONNECTED, URHO3D_HANDLER(ServerApplication, HandleClientDisonnected));
    SubscribeToEvent(E_CLIENTIDENTITY, URHO3D_HANDLER(ServerApplication, HandleClientIdentity));
    SubscribeToEvent(E_NETWORKMESSAGE, URHO3D_HANDLER(ServerApplication, HandleNetworkMessage));
    SubscribeToEvent(E_FILECHANGED, URHO3D_HANDLER(ServerApplication, HandleFileChanged));
    SubscribeToEvent(E_EXITREQUESTED, URHO3D_HANDLER(ServerApplication, HandleExitRequested));
}

// ----------------------------------------------------------------------------
void ServerApplication::LoadMap(const String& fileName)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    XMLFile* file = cache->GetResource<XMLFile>(fileName);
    if (file == NULL)
    {
        URHO3D_LOGERRORF("Failed to load map \"%s\"", fileName.CString());
        return;
    }

    scene_ = new Scene(context_);
    scene_->CreateComponent<Octree>(LOCAL);

    MapState* mapState = scene_->CreateComponent<MapState>(LOCAL);
    mapState->LoadXML(file->GetRoot());

    Node* lightNode = scene_->CreateChild("Light");
    Light* light = lightNode->CreateComponent<Light>();
    light->SetLightType(LIGHT_DIRECTIONAL);
    light->SetBrightness(3);
    lightNode->SetRotation(Quaternion(30, 30, 0));
}

// ----------------------------------------------------------------------------
void ServerApplication::CreatePlayer()
{
    Node* playerNode = scene_->CreateChild("Player");
    playerNode->CreateComponent<Player>();
    playerNode->SetPosition(Vector3(13, 0, 10));
}

// ----------------------------------------------------------------------------
void ServerApplication::HandleConnectFailed(StringHash eventType, VariantMap& eventData)
{
}

// ----------------------------------------------------------------------------
void ServerApplication::HandleClientConnected(StringHash eventType, VariantMap& eventData)
{
    using namespace ClientConnected;

    // Need to set the scene on incomming connections to enable scene replication
    Connection* connection = static_cast<Connection*>(eventData[P_CONNECTION].GetPtr());
    connection->SetScene(scene_);
}

// ----------------------------------------------------------------------------
void ServerApplication::HandleClientDisonnected(StringHash eventType, VariantMap& eventData)
{
    using namespace ClientDisconnected;

    Connection* connection = static_cast<Connection*>(eventData[P_CONNECTION].GetPtr());
    ConnectedUsers::Iterator it = connectedUsers_.Find(connection);
    if (it == connectedUsers_.End())
        return;

    VectorBuffer buffer;
    buffer.WriteUByte(Chat::GLOBAL);
    buffer.WriteString("<-- " + it->second_.name_ + " left the server!");
    GetSubsystem<Network>()->BroadcastMessage(MSG_CHATMESSAGE, true, false, buffer);

    connectedUsers_.Erase(it);
}

// ----------------------------------------------------------------------------
void ServerApplication::HandleClientIdentity(StringHash eventType, VariantMap& eventData)
{
    using namespace ClientIdentity;

    Connection* connection = static_cast<Connection*>(eventData[P_CONNECTION].GetPtr());
    const VariantMap& identity = connection->GetIdentity();
    Variant* var = identity["Username"];
    if (var == NULL)
    {
        URHO3D_LOGERROR("Client didn't provide username!");
        connection->Disconnect();
        return;
    }

    String username = var->GetString();
    if (username.Empty())
    {
        URHO3D_LOGERROR("Client provided an empty username!");
        connection->Disconnect();
        return;
    }

    for (ConnectedUsers::ConstIterator it = connectedUsers_.Begin(); it != connectedUsers_.End(); ++it)
    {
        if (it->second_.name_ == username)
        {
            eventData[P_ALLOW] = false;
            connection->Disconnect();
            return;
        }
    }

    connectedUsers_[connection] = {
        username
    };

    VectorBuffer buffer;
    buffer.WriteUByte(Chat::GLOBAL);
    buffer.WriteString("--> " + username + " joined the server!");
    GetSubsystem<Network>()->BroadcastMessage(MSG_CHATMESSAGE, true, false, buffer);
}

// ----------------------------------------------------------------------------
void ServerApplication::HandleNetworkMessage(StringHash eventType, VariantMap& eventData)
{
    using namespace NetworkMessage;
    int messageID = eventData[P_MESSAGEID].GetInt();
    Connection* connection = static_cast<Connection*>(eventData[P_CONNECTION].GetPtr());

    if (messageID == MSG_CHATMESSAGE)
    {
        MemoryBuffer buffer(eventData[P_DATA].GetBuffer());
        unsigned char messageTarget = buffer.ReadUByte();
        String message = buffer.ReadString();
        URHO3D_LOGDEBUGF("Received message \"%s\"", message.CString());

        // Append username to it, then broadcast
        message = "<" + connectedUsers_[connection].name_ + "> " + message;

        if (messageTarget & Chat::GLOBAL)
        {
            VectorBuffer outBuffer;
            outBuffer.WriteUByte(messageTarget);
            outBuffer.WriteString(message);
            GetSubsystem<Network>()->BroadcastMessage(MSG_CHATMESSAGE, true, false, outBuffer);
        }
    }
}

// ----------------------------------------------------------------------------
void ServerApplication::HandleFileChanged(StringHash eventType, VariantMap& eventData)
{
    if(xmlScene_ && xmlScene_->GetName() == eventData[FileChanged::P_RESOURCENAME].GetString())
    {
        URHO3D_LOGINFO("[Lightship] Reloading scene");
        if(scene_)
        {
            scene_->LoadXML(xmlScene_->GetRoot());
        }
    }
}

// ----------------------------------------------------------------------------
void ServerApplication::HandleExitRequested(StringHash eventType, VariantMap& eventData)
{
    engine_->Exit();
}
