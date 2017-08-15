#include "lightship/Map.h"
#include "lightship/MapState.h"
#include "lightship-server/ServerApplication.h"
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Light.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Resource/ResourceEvents.h>
#include <Urho3D/Network/Network.h>
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

    //LoadScene();
    SubscribeToEvents();
    GetSubsystem<Network>()->StartServer(1337);
}

// ----------------------------------------------------------------------------
void ServerApplication::Stop()
{
    GetSubsystem<Network>()->StopServer();
}

// ----------------------------------------------------------------------------
void ServerApplication::LoadScene()
{
    scene_ = new Scene(context_);
    scene_->CreateComponent<Octree>();

    ResourceCache* cache = GetSubsystem<ResourceCache>();
    XMLFile* file = cache->GetResource<XMLFile>("Maps/(10) Adam.xml");
    MapState* mapState = new MapState(context_);
    mapState->LoadXML(file->GetRoot());
    Map* map = scene_->CreateComponent<Map>();
    map->SetState(mapState);
    map->CreateFromState();

    Node* lightNode = scene_->CreateChild("Light");
    Light* light = lightNode->CreateComponent<Light>();
    light->SetLightType(LIGHT_DIRECTIONAL);
    light->SetBrightness(3);
    lightNode->SetRotation(Quaternion(30, 30, 0));
}

// ----------------------------------------------------------------------------
void ServerApplication::SubscribeToEvents()
{
    SubscribeToEvent(E_FILECHANGED, URHO3D_HANDLER(ServerApplication, HandleFileChanged));
    SubscribeToEvent(E_EXITREQUESTED, URHO3D_HANDLER(ServerApplication, HandleExitRequested));
}

// ----------------------------------------------------------------------------
void ServerApplication::HandleFileChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
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
void ServerApplication::HandleExitRequested(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
    engine_->Exit();
}
