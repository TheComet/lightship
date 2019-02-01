#include "Lightship/GameConfig.hpp"
#include "Lightship/Map.hpp"
#include "Lightship/MapState.hpp"
#include "Lightship/Player.hpp"
#include "Lightship/Chat/ChatServer.hpp"
#include "Lightship/Network/Protocol.hpp"
#include "Lightship/UserManager/ServerUserManager.hpp"
#include "Lightship/UserManager/User.hpp"
#include "LightshipServer/ServerApplication.hpp"
#include "LightshipServer/SignalHandler.hpp"

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

using namespace Urho3D;

namespace LS {

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
    if (ProcessCommandLine() == false)
    {
        engine_->Exit();
        return;
    }

    // configure resource cache to auto-reload resources when they change
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    cache->SetAutoReloadResources(true);

    RegisterStuff();
    SubscribeToEvents();

    GetSubsystem<Log>()->SetLevel(LOG_DEBUG);
    GetSubsystem<Network>()->StartServer(2048);

    LoadMap("Maps/(10) Adam.xml");
}

// ----------------------------------------------------------------------------
void ServerApplication::Stop()
{
    GetSubsystem<Network>()->StopServer();
}

// ----------------------------------------------------------------------------
bool ServerApplication::ProcessCommandLine()
{
    for (Vector<String>::ConstIterator it = GetArguments().Begin(); it != GetArguments().End(); ++it)
    {
        // Is it a switch?
        if (it->StartsWith("-"))
        {
            if (it->Compare("-h") == 0 || it->Compare("--help") == 0)
            {
                const char* cmd = "./lightship-server";
                fprintf(stderr, "Usage: %s [options]", cmd);
                fprintf(stderr, "  -h, --help                           = Show this help");
                return false;
            }
            else
            {
                fprintf(stderr, "Unknown option %s\n", it->CString());
                return false;
            }

            continue;
        }

        fprintf(stderr, "Unknown argument %s\n", it->CString());
        return false;
    }

    return true;
}

// ----------------------------------------------------------------------------
void ServerApplication::RegisterStuff()
{
    // Server only subsystems
    context_->RegisterSubsystem<SignalHandler>();
    context_->RegisterSubsystem<ChatServer>();
    context_->RegisterSubsystem<ServerUserManager>();
/*
    // Client/Server subsystems
    context_->RegisterSubsystem(new Script(context_));
    context_->RegisterSubsystem(new GameConfig(context_));

    // Client/Server components
    Map::RegisterObject(context_);
    MapState::RegisterObject(context_);
    Player::RegisterObject(context_);*/
}

// ----------------------------------------------------------------------------
void ServerApplication::SubscribeToEvents()
{
    SubscribeToEvent(E_FILECHANGED, URHO3D_HANDLER(ServerApplication, HandleFileChanged));
    SubscribeToEvent(E_EXITREQUESTED, URHO3D_HANDLER(ServerApplication, HandleExitRequested));
}

// ----------------------------------------------------------------------------
void ServerApplication::LoadMap(const String& fileName)
{/*
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
    lightNode->SetRotation(Quaternion(30, 30, 0));*/
}

// ----------------------------------------------------------------------------
void ServerApplication::CreatePlayer()
{/*
    Node* playerNode = scene_->CreateChild("Player");
    playerNode->CreateComponent<Player>();
    playerNode->SetPosition(Vector3(13, 0, 10));*/
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

}

URHO3D_DEFINE_APPLICATION_MAIN(LS::ServerApplication)
