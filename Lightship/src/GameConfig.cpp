#include "Lightship/GameConfig.hpp"
#include "Lightship/GameConfigEvents.hpp"

#include <Urho3D/IO/Log.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Resource/ResourceEvents.h>

using namespace Urho3D;

namespace LS {

// ----------------------------------------------------------------------------
GameConfig::GameConfig(Context* context) :
    Serializable(context)
{
    memset(&player, 0, sizeof(player));

    SubscribeToEvent(E_FILECHANGED, URHO3D_HANDLER(GameConfig, HandleFileChanged));
}

// ----------------------------------------------------------------------------
void GameConfig::Open(String fileName)
{
    xml_ = GetSubsystem<ResourceCache>()->GetResource<XMLFile>(fileName);
    Reload();
}

// ----------------------------------------------------------------------------
void GameConfig::OpenXML(XMLFile* xml)
{
    xml_ = xml;
    Reload();
}

// ----------------------------------------------------------------------------
bool GameConfig::LoadXML(const Urho3D::XMLElement& source)
{
    XMLElement playerNode = source.GetChild("player");
    XMLElement rotateNode = playerNode.GetChild("rotate");
    player.rotate.speed = rotateNode.GetFloat("speed");
    player.rotate.tilt = rotateNode.GetFloat("tilt");

    return true;
}

// ----------------------------------------------------------------------------
void GameConfig::Reload()
{
    URHO3D_LOGINFO("[Config] Reloading config");

    if(!xml_)
    {
        URHO3D_LOGERROR("[Config] Config file not open");
        return;
    }

    if (LoadXML(xml_->GetRoot()) == false)
    {
        URHO3D_LOGERROR("[Config] Failed to load XML");
        return;
    }

    SendEvent(E_GAMECONFIGRELOADED, GetEventDataMap());
}

// ----------------------------------------------------------------------------
void GameConfig::HandleFileChanged(StringHash eventType, VariantMap& eventData)
{
    using namespace FileChanged;
    (void)eventType;

    if(xml_ && xml_->GetName() == eventData[P_RESOURCENAME].GetString())
    {
        Reload();
    }
}

}
