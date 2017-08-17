#pragma once

#include "lightship/config.h"
#include <Urho3D/Scene/Component.h>
#include <stdint.h>

class TileState
{
public:
    enum Type
    {
        EMPTY = 0,
        FLOOR,
        TELEPORTER,
        WALL,

        TYPE_COUNT
    };

    enum Meta
    {
        SPAWN         = 0x01,
        DENY_SPAWN    = 0x02,
        DENY_POWER_UP = 0x04,

        META_COUNT
    };

    TileState() : type_(EMPTY), meta_(0), delay_(0), teleportDestX_(-1), teleportDestY_(-1), claimerID_(-1) {}

    Type    type_;
    uint8_t meta_;
    uint8_t delay_;         // Delay in seconds until the tile becomes claimable
    int8_t  teleportDestX_;  // If tile is a teleporter, these are the
    int8_t  teleportDestY_;  // destination tile coordinates. Can be -1 if unset.
    int8_t  claimerID_;     // Player ID of who claimed the tile. -1 is unclaimed.
};

class Trigger
{
public:
    float posX_;
    float posY_;
    float radius_;
};

class LIGHTSHIP_PUBLIC_API MapState : public Urho3D::Component
{
    URHO3D_OBJECT(MapState, Urho3D::Component);

public:
    MapState(Urho3D::Context* context);
    static void RegisterObject(Urho3D::Context* context);

    TileState& GetTile(uint8_t x, uint8_t y);
    const TileState& GetTile(uint8_t x, uint8_t y) const;
    const Urho3D::PODVector<Trigger>& GetTriggers() const;
    uint8_t GetPlayerCount() const;
    uint8_t GetWidth() const;
    uint8_t GetHeight() const;

    // Serialization
    virtual bool Load(Urho3D::Deserializer& source, bool setInstanceDefault=false) override;
    virtual bool Save(Urho3D::Serializer& dest) const override;
    virtual bool LoadXML(const Urho3D::XMLElement& source, bool setInstanceDefault=false) override;
    virtual bool SaveXML(Urho3D::XMLElement& dest) const override;
    bool LoadOMG(const Urho3D::String& fileName);

    void ClearData();

    // Networking related methods
    void RequestMapState();

private:
    bool LoadOMG_v1(const Urho3D::String& fileName, Urho3D::String* error);
    bool LoadOMG_v2(const Urho3D::String& fileName, Urho3D::String* error);
    bool ReadDBPString(Urho3D::Deserializer& source, Urho3D::String* out) const;
    void ResizeMap(uint8_t x, uint8_t y);

    void HandleNetworkMessage(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

private:
    Urho3D::PODVector<TileState> tileData_;
    Urho3D::PODVector<Trigger> triggers_;
    uint8_t playerCount_;
    uint8_t width_;
    uint8_t height_;
};
