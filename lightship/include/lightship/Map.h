#pragma once

#include <Urho3D/Scene/Component.h>
#include <stdint.h>

class TileData
{
public:
    enum Type
    {
        EMPTY = 0,
        FLOOR,
        TELEPORTER,
        WALL
    };

    enum Meta
    {
        SPAWN         = 0x01,
        DENY_SPAWN    = 0x02,
        DENY_POWER_UP = 0x04,

        META_COUNT
    };

    TileData() : type_(EMPTY), meta_(0), delay_(0), teleportDestX_(-1), teleportDestY_(-1), claimerID_(-1) {}

    Type          type_;
    uint8_t meta_;
    uint8_t delay_;         // Delay in seconds until the tile becomes claimable
    int8_t teleportDestX_;  // If tile is a teleporter, these are the
    int8_t teleportDestY_;  // destination tile coordinates. Can be -1 if unset.
    int8_t  claimerID_;     // Player ID of who claimed the tile. -1 is unclaimed.
};

class Map : public Urho3D::Component
{
    URHO3D_OBJECT(Map, Urho3D::Component);

public:
    Map(Urho3D::Context* context);

    static void RegisterObject(Urho3D::Context* context);

    // Serialization
    virtual bool LoadXML(const Urho3D::XMLElement& source, bool setInstanceDefault = false) override;
    virtual bool SaveXML(Urho3D::XMLElement& dest) const override;
    bool LoadOMG(const Urho3D::String& fileName);

    TileData& TileAt(uint8_t x, uint8_t y);
    const TileData& TileAt(uint8_t x, uint8_t y) const;

    void Generate();

private:
    void ResizeMap(uint8_t x, uint8_t y);
    Urho3D::Node* CreateTile(TileData::Type type);

private:
    Urho3D::PODVector<TileData> tileData_;
    uint8_t playerCount_;
    uint8_t width_;
    uint8_t height_;
};
