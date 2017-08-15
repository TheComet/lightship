#include "lightship/DebugTextScroll.h"
#include "lightship/GameConfig.h"
#include "lightship/Lightship.h"
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
    Component(context)
{
}
/*
// ----------------------------------------------------------------------------
void Lightship::Start()
{
    // configure resource cache
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    cache->SetAutoReloadResources(true);

    LoadScene();
    CreateCamera();
    CreatePlayer();
    CreateDebugHud();

    GetSubsystem<GameConfig>()->Open("Config/GameConfig.xml");

    SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(Lightship, HandleKeyDown));
    SubscribeToEvent(E_POSTRENDERUPDATE, URHO3D_HANDLER(Lightship, HandlePostRenderUpdate));
    SubscribeToEvent(E_FILECHANGED, URHO3D_HANDLER(Lightship, HandleFileChanged));
}

// ----------------------------------------------------------------------------
void Lightship::Stop()
{
}

// ----------------------------------------------------------------------------
void Lightship::RegisterSubsystemsAndComponents(Context* context)
{
    context->RegisterSubsystem(new Script(context));
    context->RegisterSubsystem(new GameConfig(context));
#ifdef DEBUG
    context->RegisterSubsystem(new DebugTextScroll(context));
    GetSubsystem<DebugTextScroll>()->SetTextCount(20);
#endif

    Map::RegisterObject(context);
    Player::RegisterObject(context);
}*/

// ----------------------------------------------------------------------------
void Lightship::LoadScene()
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
    Node* rotateNode = GetScene()->CreateChild("Camera", LOCAL);
    Node* cameraNode = rotateNode->CreateChild("Camera Rotate", LOCAL);
    Camera* camera = cameraNode->CreateComponent<Camera>();
    camera->SetFarClip(300.0f);

    // Give the camera a viewport
    Viewport* viewport = new Viewport(context_, GetScene(), camera);
    viewport->SetDrawDebug(true);
    renderer->SetViewport(0, viewport);

    trackingCamera_ = new TrackingCamera(context_);
    trackingCamera_->SetNodes(rotateNode, cameraNode);

    rotateNode->SetPosition(Vector3(13, -5, 15));
    rotateNode->SetRotation(Quaternion(30, 0, 0));
    cameraNode->SetPosition(Vector3(0, 0, -30));
}

// ----------------------------------------------------------------------------
void Lightship::CreatePlayer()
{
    Node* playerNode = GetScene()->CreateChild("Player");
    playerNode->CreateComponent<Player>();
    playerNode->SetPosition(Vector3(13, 0, 10));
}
