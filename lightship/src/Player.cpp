#include "lightship/Player.h"
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Resource/ResourceCache.h>

using namespace Urho3D;

// ----------------------------------------------------------------------------
Player::Player(Context* context) :
    Component(context)
{
}

// ----------------------------------------------------------------------------
void Player::OnNodeSet(Node* node)
{
    if (node == NULL)
        return;

    ResourceCache* cache = GetSubsystem<ResourceCache>();
    StaticModel* model = node->CreateComponent<StaticModel>();
    model->SetModel(cache->GetResource<Model>("Models/Player.mdl"));
    model->SetMaterial(cache->GetResource<Material>("Materials/Player.xml"));
}
