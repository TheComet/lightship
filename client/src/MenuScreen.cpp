#include "lightship-client/MenuScreen.h"
#include <Urho3D/Input/Input.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/ResourceEvents.h>

using namespace Urho3D;

// ----------------------------------------------------------------------------
MenuScreen::MenuScreen(Context* context) :
    UIElement(context)
{
    Input* input = GetSubsystem<Input>();
    mouseWasVisible_ = input->IsMouseVisible();
    input->SetMouseVisible(true);

    SubscribeToEvent(E_FILECHANGED, URHO3D_HANDLER(MenuScreen, HandleFileChanged));
}

// ----------------------------------------------------------------------------
MenuScreen::~MenuScreen()
{
    GetSubsystem<Input>()->SetMouseVisible(mouseWasVisible_);
}

// ----------------------------------------------------------------------------
void MenuScreen::LoadUI(const Urho3D::String& xmlFileName)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    xml_ = cache->GetResource<XMLFile>(xmlFileName);
    if (xml_ == NULL)
    {
        URHO3D_LOGERRORF("Failed to load XML file \"%s\"", xmlFileName.CString());
        return;
    }

    ReloadUI();
}

// ----------------------------------------------------------------------------
void MenuScreen::ReloadUI()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    if (ui_ != NULL)
        RemoveChild(ui_);
    ui_ = LoadChildXML(xml_->GetRoot(), cache->GetResource<XMLFile>("UI/DefaultStyle.xml"));
}

// ----------------------------------------------------------------------------
void MenuScreen::HandleFileChanged(Urho3D::StringHash eventTYpe, Urho3D::VariantMap& eventData)
{
    using namespace FileChanged;

    if (xml_ != NULL && xml_->GetName() == eventData[P_RESOURCENAME].GetString())
    {
        ReloadUI();
    }
}

