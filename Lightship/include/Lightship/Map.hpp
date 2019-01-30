#pragma once

#include "Lightship/config.hpp"
#include <Urho3D/Scene/Component.h>

namespace LS {

class MapState;

class LIGHTSHIP_PUBLIC_API Map : public Urho3D::Component
{
    URHO3D_OBJECT(Map, Urho3D::Component);

public:
    Map(Urho3D::Context* context);
    static void RegisterObject(Urho3D::Context* context);

    /*!
     * Destroys all visible map objects (if necessary) and builds the map
     * according to the specified state. If the state object changes in the
     * future, then these changes will reflect graphically as well (e.g. tile
     * color changes, walls being added/removed, stuff like that)
     *
     * You can also specify NULL as a map state, which will simply destroy the
     * map.
     */
    void SetState(MapState* state);

protected:
    virtual void OnNodeSet(Urho3D::Node* node) override;

private:
    void CreateFromState();
    void Destroy();
    Urho3D::Node* CreateTile(int tileType);
    Urho3D::Node* CreateTrigger();

private:
    Urho3D::SharedPtr<MapState> state_;
    Urho3D::SharedPtr<Urho3D::Node> mapNode_;
};

}
