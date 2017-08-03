#include "lightship/GameConfig.h"
#include "lightship/GameConfigEvents.h"

#include <Urho3D/IO/Log.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Resource/ResourceEvents.h>

using namespace Urho3D;


// ----------------------------------------------------------------------------
GameConfig::GameConfig(Context* context) :
    Object(context)
{
    SubscribeToEvent(E_FILECHANGED, URHO3D_HANDLER(GameConfig, HandleFileChanged));
}

// ----------------------------------------------------------------------------
void GameConfig::Load(String fileName)
{
    xml_ = GetSubsystem<ResourceCache>()->GetResource<XMLFile>(fileName);
    Reload();
}

// ----------------------------------------------------------------------------
void GameConfig::LoadXML(XMLFile* xml)
{
    xml_ = xml;
    Reload();
}

// ----------------------------------------------------------------------------
void GameConfig::Reload()
{
    if(!xml_)
    {
        URHO3D_LOGERROR("[Config] Failed to load XML file");
        return;
    }

    XMLElement root = xml_->GetRoot();

    SendEvent(E_GAMECONFIGRELOADED, GetEventDataMap());
}

// ----------------------------------------------------------------------------
void GameConfig::HandleFileChanged(StringHash eventType, VariantMap& eventData)
{
    using namespace FileChanged;
    (void)eventType;

    if(xml_ && xml_->GetName() == eventData[P_RESOURCENAME].GetString())
    {
        URHO3D_LOGINFO("[Config] Reloading config");
        Reload();
    }
}
