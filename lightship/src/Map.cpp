#include "lightship/Map.h"
#include <Urho3D/Core/Context.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/IO/File.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/XMLElement.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Core/StringUtils.h>
#include <cassert>

using namespace Urho3D;

static const char* TileDataTypeStr[] = {
    "Empty",
    "Floor",
    "Teleporter",
    "Wall",
    NULL
};

static const char* TileDataMetaStr[] = {
    "Spawn",
    "Deny Spawn",
    "Deny Power Up",
    NULL
};

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
bool Map::LoadXML(const Urho3D::XMLElement& source, bool setInstanceDefault)
{
    XMLElement map = source.GetChild("map");
    playerCount_ = map.GetUInt("players");
    width_ = map.GetUInt("width");
    height_ = map.GetUInt("height");

    if (playerCount_ == 0 || playerCount_ > 20)
    {
        URHO3D_LOGERRORF("Invalid player count");
        return false;
    }

    XMLElement tile = map.GetChild("tile");
    ResizeMap(width_, height_);
    for (int y = 0; y != height_; ++y)
        for (int x = 0; x != width_; ++x)
        {
            TileData& t = TileAt(x, y);
            t = TileData();  // default construct

            String typeStr = tile.GetAttribute("type");
            for (int i = 0; TileDataTypeStr[i]; ++i)
                if (typeStr == TileDataTypeStr[i])
                {
                    t.type_ = TileData::Type(i);
                    break;
                }

            if (t.type_ == TileData::TELEPORTER)
            {
                String teleporterData = tile.GetAttribute("teleport");
                StringVector xy = teleporterData.Split(',');
                t.teleportDestX_ = ToInt(xy[0]);
                t.teleportDestY_ = ToInt(xy[1]);
            }

            if (tile.HasAttribute("delay"))
                t.delay_ = tile.GetInt("delay");
            if (tile.HasAttribute("claimer"))
                t.claimerID_ = tile.GetInt("claimer");

            StringVector metaData = tile.GetAttribute("meta").Split(',');
            for (StringVector::ConstIterator it = metaData.Begin(); it != metaData.End(); ++it)
            {
                for (int i = 0; TileDataMetaStr[i]; ++i)
                    if (*it == TileDataMetaStr[i])
                        t.meta_ |= (1u << i);
            }

            tile = tile.GetNext("tile");
        }

    return true;
}

// ----------------------------------------------------------------------------
bool Map::SaveXML(Urho3D::XMLElement& dest) const
{
    XMLElement map = dest.CreateChild("map");
    map.SetUInt("players", playerCount_);
    map.SetUInt("width", width_);
    map.SetUInt("height", height_);

    for (int y = 0; y != height_; ++y)
        for (int x = 0; x != width_; ++x)
        {
            const TileData& tileData = TileAt(x, y);

            XMLElement tile = map.CreateChild("tile");
            tile.SetAttribute("type", TileDataTypeStr[tileData.type_]);

            // Only save teleport data if tile is a teleporter
            if (tileData.type_ == TileData::TELEPORTER)
            {
                tile.SetAttribute("teleport", ToString("%d,%d", tileData.teleportDestX_, tileData.teleportDestY_));
            }

            // Only save delay and claimerID if they are non-default
            if (tileData.delay_ > 0)
                tile.SetUInt("delay", tileData.delay_);
            if (tileData.claimerID_ > -1)
                tile.SetInt("claimer", tileData.claimerID_);

            // Convert meta data into a comma-separated list of strings
            String metaData;
            for (int i = 0; i != TileData::META_COUNT; ++i)
            {
                if (tileData.meta_ & (1u << i))
                {
                    if (metaData.Empty() == false)
                        metaData.Append(",");
                    metaData.Append(TileDataMetaStr[i]);
                }
            }
            if (metaData.Empty() == false)
                tile.SetAttribute("meta", metaData);
        }

    return true;
}

// ----------------------------------------------------------------------------
bool Map::LoadOMG(const String& fileName)
{
    File file(context_, fileName);
    if (file.IsOpen() == false)
        return false;

#define TRY_READ(buffer, bytes, errorMessage) \
    if (file.Read(buffer, bytes) != bytes)    \
    {                                         \
        URHO3D_LOGERROR(errorMessage);        \
        return false;                         \
    }

#define ENFORCE_MAX(value, maxValue, errorMessage) \
    if (value > maxValue)                          \
    {                                              \
        URHO3D_LOGERRORF("%s. Max is %d, read value was %d", errorMessage, maxValue, value); \
        return false;                              \
    }

    uint8_t version;
    TRY_READ(&version, 1, "Failed to read version byte")

    uint32_t playerCount;
    TRY_READ(&playerCount, 4, "Failed to read player count")
    ENFORCE_MAX(playerCount, 20u, "Player count is too high")

    uint32_t width, height;
    TRY_READ(&width, 4, "Failed to read map width")
    TRY_READ(&height, 4, "Failed to read map height")
    ENFORCE_MAX(width, 255u, "Map width is too high")
    ENFORCE_MAX(height, 255u, "Map height is too high")

    playerCount_ = playerCount;
    ResizeMap(width, height);

    // Old format embeds a PNG image. The first 4 bytes should specify the
    // length of this block of data. Simply skip it.
    uint32_t pngSize;
    TRY_READ(&pngSize, 4, "Failed to read embedded PNG size")
    file.Seek(17 + pngSize);  // 17 bytes read so far

    for (int x = 0; x != width_; ++x)
        for (int y = 0; y != height_; ++y)
        {
            uint8_t tileType;
            TRY_READ(&tileType, 1, "Failed to read tile type")
            ENFORCE_MAX(tileType, 4u, "Invalid tile type")

            int32_t teleportX, teleportY;
            TRY_READ(&teleportX, 4, "Failed to read teleport X destination")
            TRY_READ(&teleportY, 4, "Failed to read teleport Y destination")
            ENFORCE_MAX(teleportX, 255, "Invalid teleport X destination")
            ENFORCE_MAX(teleportY, 255, "Invalid teleport X destination")

            TileAt(x, y) = TileData();
            // In the old map format, spawn locations (type 4) had their own tile type. In our format those are stored as meta info
            TileAt(x, y).type_ = tileType < 4 ? TileData::Type(tileType) : TileData::FLOOR;
            TileAt(x, y).meta_ |= tileType == 4 ? TileData::SPAWN : 0;
            TileAt(x, y).teleportDestX_ = teleportX;
            TileAt(x, y).teleportDestY_ = teleportY;
        }

    return true;
}

// ----------------------------------------------------------------------------
TileData& Map::TileAt(uint8_t x, uint8_t y)
{
    assert(x < width_ && y < height_);
    return tileData_[y*width_+x];
}

// ----------------------------------------------------------------------------
const TileData& Map::TileAt(uint8_t x, uint8_t y) const
{
    assert(x < width_ && y < height_);
    return tileData_[y*width_+x];
}

// ----------------------------------------------------------------------------
void Map::Generate()
{
    for (int y = 0; y != height_; ++y)
        for (int x = 0; x != width_; ++x)
        {
            const TileData& tile = TileAt(x, y);

            Node* node = CreateTile(tile.type_);
            if (node == NULL)
                continue;

            node->SetPosition(Vector3(x, 0, y));
        }
}

// ----------------------------------------------------------------------------
void Map::ResizeMap(uint8_t x, uint8_t y)
{
    width_ = x;
    height_ = y;
    tileData_.Resize(width_ * height_);
}

// ----------------------------------------------------------------------------
Node* Map::CreateTile(TileData::Type type)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    Node* node = NULL;

    switch (type)
    {
        case TileData::EMPTY:
            break;

        case TileData::FLOOR:
        {
            node = GetScene()->CreateChild("Floor");
            StaticModel* model = node->CreateComponent<StaticModel>();
            model->SetModel(cache->GetResource<Model>("Models/Tile.mdl"));
            model->SetMaterial(cache->GetResource<Material>("Materials/Tile.xml"));
        } break;

        case TileData::TELEPORTER:
        {
            node = GetScene()->CreateChild("Teleporter");
            StaticModel* model = node->CreateComponent<StaticModel>();
            model->SetModel(cache->GetResource<Model>("Models/Teleporter.mdl"));
            model->SetMaterial(cache->GetResource<Material>("Materials/Teleporter.xml"));
        } break;

        case TileData::WALL:
        {
            node = GetScene()->CreateChild("Wall");
            StaticModel* model = node->CreateComponent<StaticModel>();
            model->SetModel(cache->GetResource<Model>("Models/Wall.mdl"));
            model->SetMaterial(cache->GetResource<Material>("Materials/Wall.xml"));
        } break;
    }

    return node;
}
