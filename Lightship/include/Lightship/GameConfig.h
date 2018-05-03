#pragma once

#include "Lightship/config.h"
#include <Urho3D/Scene/Serializable.h>

namespace Urho3D {
    class Context;
    class XMLFile;
}

class LIGHTSHIP_PUBLIC_API GameConfig : public Urho3D::Serializable
{
    URHO3D_OBJECT(GameConfig, Urho3D::Serializable)

public:
    GameConfig(Urho3D::Context* context);

    void Open(Urho3D::String fileName);
    void OpenXML(Urho3D::XMLFile* xml);

    virtual bool LoadXML(const Urho3D::XMLElement& source) override;
    void Reload();

public:
    struct PlayerData
    {
        struct Rotate
        {
            float speed;
            float tilt;
        } rotate;
    } player;

private:
    void HandleFileChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

    Urho3D::SharedPtr<Urho3D::XMLFile> xml_;
};
