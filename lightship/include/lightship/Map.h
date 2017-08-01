#pragma once

#include <Urho3D/Scene/Component.h>

class MapState;

class Map : public Urho3D::Component
{
    URHO3D_OBJECT(Map, Urho3D::Component);

public:
    Map(Urho3D::Context* context);
    static void RegisterObject(Urho3D::Context* context);

    void SetState(MapState* state);
    void CreateFromState();
    void Destroy();

protected:
    virtual void OnNodeSet(Urho3D::Node* node) override;

private:
    Urho3D::Node* CreateTile(int tileType);
    Urho3D::Node* CreateTrigger();

private:
    Urho3D::SharedPtr<MapState> state_;
    Urho3D::SharedPtr<Urho3D::Node> mapNode_;
};
