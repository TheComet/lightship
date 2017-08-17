#include "lightship/MapState.h"
#include "lightship/Protocol.h"
#include <Urho3D/Core/Context.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/IO/File.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/IO/MemoryBuffer.h>
#include <Urho3D/Network/Network.h>
#include <Urho3D/Network/NetworkEvents.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/XMLElement.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Core/StringUtils.h>
#include <cassert>

using namespace Urho3D;

static const char* tileDataTypeStr[] = {
    "Empty",
    "Floor",
    "Teleporter",
    "Wall",
    NULL
};

static const char* tileDataMetaStr[] = {
    "Spawn",
    "Deny Spawn",
    "Deny Power Up",
    NULL
};

enum NetworkMessageAction
{
    REQUEST_STATE = 0,
    RECEIVE_STATE,
    DELTA
};

// ----------------------------------------------------------------------------
MapState::MapState(Context* context) :
    Component(context),
    playerCount_(0),
    width_(0),
    height_(0)
{
    SubscribeToEvent(E_NETWORKMESSAGE, URHO3D_HANDLER(MapState, HandleNetworkMessage));
}

// ----------------------------------------------------------------------------
void MapState::RegisterObject(Context* context)
{
    context->RegisterFactory<MapState>("Lightship");
}

// ----------------------------------------------------------------------------
TileState& MapState::GetTile(uint8_t x, uint8_t y)
{
    assert(x < width_ && y < height_);
    return tileData_[y*width_+x];
}

// ----------------------------------------------------------------------------
const TileState& MapState::GetTile(uint8_t x, uint8_t y) const
{
    assert(x < width_ && y < height_);
    return tileData_[y*width_+x];
}

// ----------------------------------------------------------------------------
const PODVector<Trigger>& MapState::GetTriggers() const
{
    return triggers_;
}

// ----------------------------------------------------------------------------
uint8_t MapState::GetPlayerCount() const
{
    return playerCount_;
}

// ----------------------------------------------------------------------------
uint8_t MapState::GetWidth() const
{
    return width_;
}

// ----------------------------------------------------------------------------
uint8_t MapState::GetHeight() const
{
    return height_;
}

// ----------------------------------------------------------------------------
bool MapState::Load(Urho3D::Deserializer& source, bool setInstanceDefault)
{
    ClearData();

    playerCount_ = source.ReadUByte();
    width_ = source.ReadUByte();
    height_ = source.ReadUByte();

    if (playerCount_ == 0 || playerCount_ > 20)
    {
        URHO3D_LOGERRORF("Invalid player count");
        return false;
    }

    ResizeMap(width_, height_);
    for (int y = 0; y != height_; ++y)
        for (int x = 0; x != width_; ++x)
        {
            TileState& t = GetTile(x, y);
            t = TileState();  // default construct

            uint8_t type = source.ReadUByte();
            if (type > TileState::TYPE_COUNT)
            {
                URHO3D_LOGERROR("Invalid tile type read");
                return false;
            }
            t.type_ = static_cast<TileState::Type>(type);

            // Load teleporter destination data
            if (t.type_ == TileState::TELEPORTER)
            {
                t.teleportDestX_ = source.ReadByte();
                t.teleportDestY_ = source.ReadByte();
            }

            t.delay_     = source.ReadUByte();
            t.claimerID_ = source.ReadByte();
            t.meta_      = source.ReadUByte();
            if (t.meta_ > TileState::META_COUNT)
            {
                URHO3D_LOGERROR("Invalid meta data read");
                return false;
            }
        }

    // Triggers
    uint8_t triggerCount = source.ReadUByte();
    for (uint8_t i = 0; i != triggerCount; ++i)
    {
        triggers_.Push(Trigger());
        triggers_.Back().posX_   = source.ReadFloat();
        triggers_.Back().posY_   = source.ReadFloat();
        triggers_.Back().radius_ = source.ReadFloat();
    }

    return true;
}

// ----------------------------------------------------------------------------
bool MapState::Save(Urho3D::Serializer& dest) const
{
    dest.WriteUByte(playerCount_);
    dest.WriteUByte(width_);
    dest.WriteUByte(height_);

    for (int y = 0; y != height_; ++y)
        for (int x = 0; x != width_; ++x)
        {
            const TileState& tile = GetTile(x, y);

            dest.WriteUByte(tile.type_);

            // Only save teleport data if tile is a teleporter
            if (tile.type_ == TileState::TELEPORTER)
            {
                dest.WriteByte(tile.teleportDestX_);
                dest.WriteByte(tile.teleportDestY_);
            }

            dest.WriteUByte(tile.delay_);
            dest.WriteByte(tile.claimerID_);
            dest.WriteUByte(tile.meta_);
        }

    // Save triggers
    dest.WriteUByte(triggers_.Size());
    for (PODVector<Trigger>::ConstIterator it = triggers_.Begin(); it != triggers_.End(); ++it)
    {
        dest.WriteFloat(it->posX_);
        dest.WriteFloat(it->posY_);
        dest.WriteFloat(it->radius_);
    }

    return true;
}

// ----------------------------------------------------------------------------
bool MapState::LoadXML(const Urho3D::XMLElement& source, bool setInstanceDefault)
{
    ClearData();

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
            TileState& t = GetTile(x, y);
            t = TileState();  // default construct

            // Tile type
            String typeStr = tile.GetAttribute("type");
            for (int i = 0; tileDataTypeStr[i]; ++i)
                if (typeStr == tileDataTypeStr[i])
                {
                    t.type_ = TileState::Type(i);
                    break;
                }

            // Load teleporter destination data
            if (t.type_ == TileState::TELEPORTER)
            {
                String teleporterData = tile.GetAttribute("teleport");
                StringVector xy = teleporterData.Split(',');
                t.teleportDestX_ = ToInt(xy[0]);
                t.teleportDestY_ = ToInt(xy[1]);
            }

            // Tile delay and who owns it
            if (tile.HasAttribute("delay"))
                t.delay_ = tile.GetInt("delay");
            if (tile.HasAttribute("claimer"))
                t.claimerID_ = tile.GetInt("claimer");

            // Tile metadata
            StringVector metaData = tile.GetAttribute("meta").Split(',');
            for (StringVector::ConstIterator it = metaData.Begin(); it != metaData.End(); ++it)
            {
                for (int i = 0; tileDataMetaStr[i]; ++i)
                    if (*it == tileDataMetaStr[i])
                        t.meta_ |= (1u << i);
            }

            tile = tile.GetNext("tile");
        }

    // Triggers
    for (XMLElement trigger = map.GetChild("trigger"); trigger; trigger = trigger.GetNext("trigger"))
    {
        triggers_.Push(Trigger());
        triggers_.Back().posX_ = trigger.GetFloat("x");
        triggers_.Back().posY_ = trigger.GetFloat("y");
        triggers_.Back().radius_ = trigger.GetFloat("radius");
    }

    return true;
}

// ----------------------------------------------------------------------------
bool MapState::SaveXML(Urho3D::XMLElement& dest) const
{
    XMLElement map = dest.CreateChild("map");
    map.SetUInt("players", playerCount_);
    map.SetUInt("width", width_);
    map.SetUInt("height", height_);

    for (int y = 0; y != height_; ++y)
        for (int x = 0; x != width_; ++x)
        {
            const TileState& tileData = GetTile(x, y);

            XMLElement tile = map.CreateChild("tile");
            tile.SetAttribute("type", tileDataTypeStr[tileData.type_]);

            // Only save teleport data if tile is a teleporter
            if (tileData.type_ == TileState::TELEPORTER)
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
            for (int i = 0; i != TileState::META_COUNT; ++i)
            {
                if (tileData.meta_ & (1u << i))
                {
                    if (metaData.Empty() == false)
                        metaData.Append(",");
                    metaData.Append(tileDataMetaStr[i]);
                }
            }
            if (metaData.Empty() == false)
                tile.SetAttribute("meta", metaData);
        }

    // Save triggers
    for (PODVector<Trigger>::ConstIterator it = triggers_.Begin(); it != triggers_.End(); ++it)
    {
        XMLElement trigger = map.CreateChild("trigger");
        trigger.SetFloat("x", it->posX_);
        trigger.SetFloat("y", it->posY_);
        trigger.SetFloat("radius", it->radius_);
    }

    return true;
}

// ----------------------------------------------------------------------------
bool MapState::LoadOMG(const String& fileName)
{
    ClearData();

    String error;
    if (LoadOMG_v1(fileName, &error) == true) return true;
    if (LoadOMG_v2(fileName, &error) == true) return true;

    URHO3D_LOGERROR(error.CString());
    return false;
}

// ----------------------------------------------------------------------------
bool MapState::LoadOMG_v1(const Urho3D::String& fileName, String* error)
{
    File file(context_, fileName);
    if (file.IsOpen() == false)
        return false;

#define TRY_READ(buffer, bytes, errorMessage) \
    if (file.Read(buffer, bytes) != bytes)    \
    {                                         \
        *error = errorMessage;                \
        return false;                         \
    }

#define ENFORCE_MAX(value, maxValue, errorMessage) \
    if (value > maxValue)                          \
    {                                              \
        error->AppendWithFormat("%s. Max is %d, read value was %d", errorMessage, maxValue, value); \
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

            GetTile(x, y) = TileState();
            // In the old map format, spawn locations (type 4) had their own tile type. In our format those are stored as meta info
            GetTile(x, y).type_ = tileType < 4 ? TileState::Type(tileType) : TileState::FLOOR;
            GetTile(x, y).meta_ |= tileType == 4 ? TileState::SPAWN : 0;
            GetTile(x, y).teleportDestX_ = teleportX;
            GetTile(x, y).teleportDestY_ = teleportY;
        }

    return true;
}

// ----------------------------------------------------------------------------
bool MapState::LoadOMG_v2(const Urho3D::String& fileName, String* error)
{
    File file(context_, fileName);
    if (file.IsOpen() == false)
        return false;

#define TRY_READ(buffer, bytes, errorMessage) \
    if (file.Read(buffer, bytes) != bytes)    \
    {                                         \
        *error = errorMessage;                \
        return false;                         \
    }

#define ENFORCE_MAX(value, maxValue, errorMessage) \
    if (value > maxValue)                          \
    {                                              \
        error->AppendWithFormat("%s. Max is %d, read value was %d", errorMessage, maxValue, value); \
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
            ENFORCE_MAX(teleportY, 255, "Invalid teleport Y destination")

            // Not sure what this is (probably a mask saying where powerups are allowed to spawn and where not)
            uint8_t flag;
            TRY_READ(&flag, 1, "Failed to read flag byte");

            GetTile(x, y) = TileState();
            // In the old map format, spawn locations (type 4) had their own tile type. In our format those are stored as meta info
            GetTile(x, y).type_ = tileType < 4 ? TileState::Type(tileType) : TileState::FLOOR;
            GetTile(x, y).meta_ |= (tileType == 4 ? TileState::SPAWN : 0);
            // I believe 0x02 stored the tiles that denied power up spawning,
            // and 0x01 stored the tiles that players couldn't spawn on.
            GetTile(x, y).meta_ |= (flag & 0x01 ? TileState::DENY_SPAWN : 0);
            GetTile(x, y).meta_ |= (flag & 0x02 ? TileState::DENY_POWER_UP : 0);
            GetTile(x, y).teleportDestX_ = teleportX;
            GetTile(x, y).teleportDestY_ = teleportY;
        }

    // We aren't actually interested in the scripts, but have to read them to skip them
    uint8_t hasStartupScript;
    TRY_READ(&hasStartupScript, 1, "Failed to read startup script flag");
    ENFORCE_MAX(hasStartupScript, 1, "Invalid startup script flag");
    if (hasStartupScript)
    {
        uint32_t lineCount, px, py;
        TRY_READ(&lineCount, 4, "Failed to read line count");
        ENFORCE_MAX(lineCount, 1024, "Line count seems suspiciously high...");
        TRY_READ(&px, 4, "Failed to read editor X location");
        TRY_READ(&py, 4, "Failed to read editor Y location");

        // DBP goes from 0 to l inclusive
        for (int l = -1; l != lineCount; ++l)
            if (ReadDBPString(file, NULL) == false)
                return false;
    }

    uint8_t triggerCount;
    TRY_READ(&triggerCount, 1, "Failed to read trigger count");
    for (int t = 0; t != triggerCount; ++t)
    {
        uint32_t lineCount, px, py, bindTrigger, radius;
        TRY_READ(&lineCount, 4, "Failed to read line count");
        ENFORCE_MAX(lineCount, 1024, "Line count seems suspiciously high...");
        TRY_READ(&px, 4, "Failed to read editor trigger X location");
        TRY_READ(&py, 4, "Failed to read editor trigger Y location");

        TRY_READ(&bindTrigger, 4, "Failed to read bind trigger");
        TRY_READ(&px, 4, "Failed to read trigger X location");
        TRY_READ(&py, 4, "Failed to read trigger Y location");
        TRY_READ(&radius, 4, "Failed to read trigger radius");

        triggers_.Push(Trigger());
        triggers_.Back().posX_ = px/20.0 - 2.0;
        triggers_.Back().posY_ = height_ - py/20.0 + 1;
        triggers_.Back().radius_ = radius/20.0;

        // DBP goes from 0 to l inclusive
        for (int l = -1; l != lineCount; ++l)
            if (ReadDBPString(file, NULL) == false)
                return false;
    }

    return true;
}

// ----------------------------------------------------------------------------
bool MapState::ReadDBPString(Urho3D::Deserializer& source, String* out) const
{
    // The DBP command "read string" reads bytes until it hits a CRLF
    char buf;
    while (true)
    {
        if (source.Read(&buf, 1) != 1)
            return false;

        if (buf == '\r')
        {
            // expecting a newline to follow carriage
            if (source.Read(&buf, 1) != 1)
                return false;
            if (buf != '\n')
                return false;
            return true;
        }

        // if we ever hit a NULL, something went wrong
        if (buf == '\0')
            return false;

        if (out != NULL)
            out->Append(buf);
    }
}

// ----------------------------------------------------------------------------
void MapState::ClearData()
{
    triggers_.Clear();
    tileData_.Clear();
    width_ = 0;
    height_ = 0;
    playerCount_ = 0;
}

// ----------------------------------------------------------------------------
void MapState::ResizeMap(uint8_t x, uint8_t y)
{
    width_ = x;
    height_ = y;
    tileData_.Resize(width_ * height_);
}

// ----------------------------------------------------------------------------
void MapState::RequestMapState()
{
    Connection* connection = GetSubsystem<Network>()->GetServerConnection();
    if (connection == NULL)
    {
        URHO3D_LOGERROR("Attempted to request map state, but there is no connection to the server!");
        return;
    }

    URHO3D_LOGDEBUG("Requesting map state...");
    VectorBuffer buffer;
    buffer.WriteUByte(REQUEST_STATE);
    connection->SendMessage(MSG_MAPSTATE, true, false, buffer);
}
#include "lightship/Map.h"
// ----------------------------------------------------------------------------
void MapState::HandleNetworkMessage(StringHash eventType, VariantMap& eventData)
{
    using namespace NetworkMessage;
    int messageID = eventData[P_MESSAGEID].GetInt();

    if (messageID != MSG_MAPSTATE)
        return;

    MemoryBuffer buffer(eventData[P_DATA].GetBuffer());
    NetworkMessageAction action = static_cast<NetworkMessageAction>(buffer.ReadUByte());
    switch (action)
    {
        case REQUEST_STATE:
        {
            URHO3D_LOGDEBUG("Sending map...");
            VectorBuffer stateBuffer;
            stateBuffer.WriteUByte(RECEIVE_STATE);
            Save(stateBuffer);

            Connection* connection = static_cast<Connection*>(eventData[P_CONNECTION].GetPtr());
            connection->SendMessage(MSG_MAPSTATE, true, true, stateBuffer);
        } break;

        case RECEIVE_STATE:
        {
            URHO3D_LOGDEBUG("Receiving map state...");
            Load(buffer);
            URHO3D_LOGDEBUGF("Received map is %dx%d with max %d players", GetWidth(), GetHeight(), GetPlayerCount());
        } break;

        case DELTA:
        {

        } break;

        default:
            URHO3D_LOGERROR("Received an invalid map state network action!");
            break;
    }
}
