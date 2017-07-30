#include "lightship/Lightship.h"
#include "lightship/DebugTextScroll.h"
#include "lightship/Config.h"

#include <Urho3D/AngelScript/Script.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Engine/DebugHud.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/RenderPath.h>
#include <Urho3D/Graphics/Viewport.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/LuaScript/LuaScript.h>
#include <Urho3D/Math/Random.h>
#include <Urho3D/Network/Network.h>
#include <Urho3D/Network/NetworkEvents.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/ResourceEvents.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/AngelScript/ScriptFile.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/Window.h>

using namespace Urho3D;

// ----------------------------------------------------------------------------
Lightship::Lightship(Context* context) :
    Application(context)
{
}

// ----------------------------------------------------------------------------
void Lightship::Setup()
{
    // called before engine initialization

    engineParameters_["WindowTitle"] = "Lightship";
    engineParameters_["FullScreen"]  = false;
    engineParameters_["Headless"]    = false;
    engineParameters_["Multisample"] = 2;
    engineParameters_["VSync"] = true;

    engineParameters_["WindowResizable"] = true;
}

// ----------------------------------------------------------------------------
void Lightship::Start()
{
    // configure resource cache
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    cache->SetAutoReloadResources(true);

    RegisterSubsystems();
    CreateDebugHud();

    GetSubsystem<Config>()->Load("Config/Config.xml");

    SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(Lightship, HandleKeyDown));
    SubscribeToEvent(E_POSTRENDERUPDATE, URHO3D_HANDLER(Lightship, HandlePostRenderUpdate));
    SubscribeToEvent(E_FILECHANGED, URHO3D_HANDLER(Lightship, HandleFileChanged));
}

// ----------------------------------------------------------------------------
void Lightship::Stop()
{
}

// ----------------------------------------------------------------------------
void Lightship::RegisterSubsystems()
{
    context_->RegisterSubsystem(new Script(context_));
    context_->RegisterSubsystem(new Config(context_));
#ifdef DEBUG
    context_->RegisterSubsystem(new DebugTextScroll(context_));
    GetSubsystem<DebugTextScroll>()->SetTextCount(20);
#endif
}

// ----------------------------------------------------------------------------
void Lightship::LoadScene(const String& fileName)
{
}

// ----------------------------------------------------------------------------
void Lightship::CreateCamera()
{
    Renderer* renderer = GetSubsystem<Renderer>();
    if(renderer == NULL)
        return;

    /*
     * The camera is attached to a "rotate node", which is in turn attached to
     * a "move" node. The rotation controller is separate from the movement
     * controller.
     */
    playerNode_   = scene_->CreateChild("Player", LOCAL);
    cameraOffsetNode_ = playerNode_->CreateChild("Camera Offset", LOCAL);
    cameraRotateNode_ = cameraOffsetNode_->CreateChild("Camera Rotate", LOCAL);
    Camera* camera = cameraRotateNode_->CreateComponent<Camera>(LOCAL);
    camera->SetFarClip(300.0f);

    // Give the camera a viewport
    Viewport* viewport = new Viewport(context_, scene_, camera);
    viewport->SetDrawDebug(true);
    renderer->SetViewport(0, viewport);
}

// ----------------------------------------------------------------------------
void Lightship::CreatePlayer()
{
    // Needs to always exist
    Node* modelNode = playerNode_->CreateChild("Player Model", LOCAL);
}

// ----------------------------------------------------------------------------
void Lightship::CreateDebugHud()
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
void Lightship::HandleKeyDown(StringHash eventType, VariantMap& eventData)
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
void Lightship::HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData)
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

// ----------------------------------------------------------------------------
void Lightship::HandleFileChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
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
