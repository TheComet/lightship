#include "lightship-client/ClientApplication.h"
#include "lightship-client/MainMenu.h"
#include "lightship/DebugTextScroll.h"
#include "lightship/GameConfig.h"
#include "lightship/Map.h"
#include "lightship/MapState.h"
#include "lightship/Player.h"
#include "lightship/TrackingCamera.h"
#include <Urho3D/AngelScript/Script.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Engine/DebugHud.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/Viewport.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/Network/Network.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/UI/UI.h>

using namespace Urho3D;

// ----------------------------------------------------------------------------
ClientApplication::ClientApplication(Context* context) :
    Application(context),
    debugDrawMode_(DRAW_NONE)
{
}

// ----------------------------------------------------------------------------
void ClientApplication::Setup()
{
    engineParameters_["FullScreen"]      = false;
    engineParameters_["WindowResizable"] = true;
    engineParameters_["VSync"]           = true;
    engineParameters_["Multisample"]     = 2;
}

// ----------------------------------------------------------------------------
void ClientApplication::Start()
{
    RegisterStuff();
    CreateDebugHud();
    SubscribeToEvents();

    GetSubsystem<ResourceCache>()->SetAutoReloadResources(true);

    MainMenu* menu = GetSubsystem<UI>()->GetRoot()->CreateChild<MainMenu>();
    menu->SetClient(this);
    //menu->SwitchToScreen(MainMenu::SCREEN_CONNECT);

    GetSubsystem<Input>()->SetMouseVisible(true);
    GetSubsystem<Log>()->SetLevel(LOG_DEBUG);
}

// ----------------------------------------------------------------------------
void ClientApplication::Stop()
{
    DisconnectFromServer();
    GetSubsystem<UI>()->GetRoot()->RemoveAllChildren();
}

// ----------------------------------------------------------------------------
void ClientApplication::RegisterStuff()
{
    // Client only subsystems
#ifdef DEBUG
    context_->RegisterSubsystem(new DebugTextScroll(context_));
    GetSubsystem<DebugTextScroll>()->SetTextCount(20);
#endif

    // Client only components
    MainMenu::RegisterObject(context_);

    // Client/Server subsystems
    context_->RegisterSubsystem(new Script(context_));
    context_->RegisterSubsystem(new GameConfig(context_));

    // Client/Server components
    Map::RegisterObject(context_);
    MapState::RegisterObject(context_);
    Player::RegisterObject(context_);
}

// ----------------------------------------------------------------------------
void ClientApplication::SubscribeToEvents()
{
    SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(ClientApplication, HandleKeyDown));
    SubscribeToEvent(E_POSTRENDERUPDATE, URHO3D_HANDLER(ClientApplication, HandlePostRenderUpdate));
}

// ----------------------------------------------------------------------------
void ClientApplication::CreateDebugHud()
{
#ifdef DEBUG
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    XMLFile* style = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");
    debugHud_ = engine_->CreateDebugHud();
    if(debugHud_)
        debugHud_->SetDefaultStyle(style);
#endif
}

// ----------------------------------------------------------------------------
void ClientApplication::ConnectToServer(const Urho3D::String& address, unsigned int port)
{
    scene_ = new Scene(context_);
    GetSubsystem<Network>()->Connect(address, port, scene_);

    scene_->CreateComponent<Octree>(LOCAL);
    MapState* mapState = scene_->CreateComponent<MapState>(LOCAL);
    Map* map = scene_->CreateComponent<Map>(LOCAL);

    map->SetState(mapState);
    mapState->RequestMapState();
}

// ----------------------------------------------------------------------------
void ClientApplication::DisconnectFromServer()
{
    Network* network = GetSubsystem<Network>();
    if (network->GetServerConnection() == NULL)
        return;

    network->Disconnect();
    scene_ = NULL;
}

// ----------------------------------------------------------------------------
void ClientApplication::CreateCamera()
{
    Renderer* renderer = GetSubsystem<Renderer>();
    if(renderer == NULL)
        return;

    /*
     * The camera is attached to a "rotate node", which is in turn attached to
     * a "move" node. The rotation controller is separate from the movement
     * controller.
     */
    Node* rotateNode = scene_->CreateChild("Camera", LOCAL);
    Node* cameraNode = rotateNode->CreateChild("Camera Rotate", LOCAL);
    Camera* camera = cameraNode->CreateComponent<Camera>();
    camera->SetFarClip(300.0f);

    // Give the camera a viewport
    Viewport* viewport = new Viewport(context_, scene_, camera);
    viewport->SetDrawDebug(true);
    renderer->SetViewport(0, viewport);

    trackingCamera_ = new TrackingCamera(context_);
    trackingCamera_->SetNodes(rotateNode, cameraNode);

    rotateNode->SetPosition(Vector3(13, -5, 15));
    rotateNode->SetRotation(Quaternion(30, 0, 0));
    cameraNode->SetPosition(Vector3(0, 0, -30));
}

// ----------------------------------------------------------------------------
void ClientApplication::HandleKeyDown(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
    using namespace KeyDown;
    int key = eventData[P_KEY].GetInt();

    if(key == KEY_ESCAPE)
    {
        engine_->Exit();
    }

    // Toggle debug geometry
#ifdef DEBUG
    if(key == KEY_F1)
    {
        switch(debugDrawMode_)
        {
            case DRAW_NONE    : debugDrawMode_ = DRAW_PHYSICS; break;
            case DRAW_PHYSICS : debugDrawMode_ = DRAW_NONE;    break;
        }
    }

    // Toggle debug HUD
    if(key == KEY_F2)
    {
        if(debugHud_->GetMode() == DEBUGHUD_SHOW_NONE)
            debugHud_->SetMode(DEBUGHUD_SHOW_ALL);
        else if(debugHud_->GetMode() == DEBUGHUD_SHOW_ALL)
            debugHud_->SetMode(DEBUGHUD_SHOW_MEMORY);
        else
            debugHud_->SetMode(DEBUGHUD_SHOW_NONE);
    }

    // Toggle mouse visibility (for debugging)
    if(key == KEY_9)
        GetSubsystem<Input>()->SetMouseVisible(!GetSubsystem<Input>()->IsMouseVisible());
#endif
}

// ----------------------------------------------------------------------------
void ClientApplication::HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData)
{
    if(scene_ == NULL)
        return;

    DebugRenderer* debugRenderer = scene_->GetComponent<DebugRenderer>();
    if(!debugRenderer)
        return;
    bool depthTest = true;

    switch(debugDrawMode_)
    {
        case DRAW_NONE: return;

        case DRAW_PHYSICS:
        {
            PhysicsWorld* phy = scene_->GetComponent<PhysicsWorld>();
            if(!phy)
                return;
            phy->DrawDebugGeometry(depthTest);
            break;
        }
    }
}
