#pragma once

#include <Urho3D/UI/UIElement.h>

class MenuScreen : public Urho3D::UIElement
{
    URHO3D_OBJECT(MenuScreen, Urho3D::UIElement)

public:
    MenuScreen(Urho3D::Context* context);
    ~MenuScreen();

    void LoadUI(const Urho3D::String& xmlFileName);
    Urho3D::SharedPtr<Urho3D::UIElement> ui_;

private:
    void ReloadUI();
    void HandleFileChanged(Urho3D::StringHash eventTYpe, Urho3D::VariantMap& eventData);

    Urho3D::SharedPtr<Urho3D::XMLFile> xml_;
    bool mouseWasVisible_;
};
