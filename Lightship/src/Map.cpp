#include "Lightship/Map.h"
#include "Lightship/MapState.h"
#include <Urho3D/Core/Context.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/XMLFile.h>

using namespace Urho3D;

// ----------------------------------------------------------------------------
Map::Map(Context* context) :
    Component(context)
{
}

// ----------------------------------------------------------------------------
void Map::RegisterObject(Context* context)
{
    context->RegisterFactory<Map>("Lightship");
}

// ----------------------------------------------------------------------------
void Map::SetState(MapState* state)
{
    Destroy();
    state_ = state;
    if (state_ != NULL)
        CreateFromState();
}

// ----------------------------------------------------------------------------
void Map::OnNodeSet(Node* node)
{
    if (node == NULL)
        Destroy();
}

// ----------------------------------------------------------------------------
void Map::CreateFromState()
{
    // All map objects are a child of this node
    mapNode_ = node_->CreateChild("Map", LOCAL);

    // Create all tiles
    for (int y = 0; y != state_->GetHeight(); ++y)
        for (int x = 0; x != state_->GetWidth(); ++x)
        {
            const TileState& tile = state_->GetTile(x, y);

            Node* node = CreateTile(tile.type_);
            if (node == NULL)
                continue;

            node->SetPosition(Vector3(x, 0, y));
        }

    // Highlight where triggers are using a wireframe sphere
    for (PODVector<Trigger>::ConstIterator it = state_->GetTriggers().Begin(); it != state_->GetTriggers().End(); ++it)
    {
        Node* node = CreateTrigger();
        node->SetPosition(Vector3(it->posX_, 0, it->posY_));
        node->SetScale(it->radius_ * 2.0);
    }
}

// ----------------------------------------------------------------------------
void Map::Destroy()
{
    if (mapNode_ == NULL)
        return;
    mapNode_->Remove();
    mapNode_ = NULL;
}

// ----------------------------------------------------------------------------
Node* Map::CreateTile(int tileType)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    Node* node = NULL;

    TileState::Type type = static_cast<TileState::Type>(tileType);
    switch (type)
    {
        case TileState::FLOOR:
        {
            node = mapNode_->CreateChild("Floor", LOCAL);
            StaticModel* model = node->CreateComponent<StaticModel>();
            model->SetModel(cache->GetResource<Model>("Models/Tile.mdl"));
            model->SetMaterial(cache->GetResource<Material>("Materials/Tile.xml"));
        } break;

        case TileState::TELEPORTER:
        {
            node = mapNode_->CreateChild("Teleporter", LOCAL);
            StaticModel* model = node->CreateComponent<StaticModel>();
            model->SetModel(cache->GetResource<Model>("Models/Teleporter.mdl"));
            model->SetMaterial(cache->GetResource<Material>("Materials/Teleporter.xml"));
        } break;

        case TileState::WALL:
        {
            node = mapNode_->CreateChild("Wall", LOCAL);
            StaticModel* model = node->CreateComponent<StaticModel>();
            model->SetModel(cache->GetResource<Model>("Models/Wall.mdl"));
            model->SetMaterial(cache->GetResource<Material>("Materials/Wall.xml"));
        } break;

        case TileState::EMPTY:
        case TileState::TYPE_COUNT:
            break;
    }

    return node;
}

// ----------------------------------------------------------------------------
Node* Map::CreateTrigger()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Node* node = mapNode_->CreateChild("Trigger", LOCAL);
    StaticModel* model = node->CreateComponent<StaticModel>();
    Material* material = cache->GetResource<Material>("Materials/DefaultGrey.xml");
    model->SetModel(cache->GetResource<Model>("Models/Sphere.mdl"));
    model->SetMaterial(material);
    material->SetFillMode(FILL_WIREFRAME);

    return node;
}
