#pragma once

#include "lightship/config.h"
#include <Urho3D/Core/Object.h>

namespace Urho3D {
    class Context;
    class XMLFile;
}

class LIGHTSHIP_PUBLIC_API GameConfig : public Urho3D::Object
{
    URHO3D_OBJECT(GameConfig, Urho3D::Object)

public:
    GameConfig(Urho3D::Context* context);

    void Load(Urho3D::String fileName);
    void LoadXML(Urho3D::XMLFile* xml);
    void Reload();

private:
    void HandleFileChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

    Urho3D::SharedPtr<Urho3D::XMLFile> xml_;
};
