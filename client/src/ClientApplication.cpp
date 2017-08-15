#include "lightship-client/ClientApplication.h"
#include "lightship/MapState.h"
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Engine/DebugHud.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/Network/Network.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Scene/Scene.h>

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
    SubscribeToEvents();

    GetSubsystem<Network>()->Connect("127.0.0.1", 1337, scene_);
}

// ----------------------------------------------------------------------------
void ClientApplication::Stop()
{
    GetSubsystem<Network>()->Disconnect();
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
