#include "Lightship/Menu/Menu.hpp"

#include <Urho3D/UI/Text.h>
#include <Urho3D/IO/Log.h>

using namespace Urho3D;

namespace LS {

// ----------------------------------------------------------------------------
void MainMenu::Handle_BTN_MAINLOCAL_CONNECT(StringHash eventType, VariantMap& eventData)
{
    SwitchToScreen(SCREEN_CONNECTPROMPT);
}
void MainMenu::Handle_BTN_MAINLOCAL_MAPEDITOR(StringHash eventType, VariantMap& eventData)
{
}
void MainMenu::Handle_BTN_MAINLOCAL_OPTIONS(StringHash eventType, VariantMap& eventData)
{
}
void MainMenu::Handle_BTN_MAINLOCAL_QUIT(StringHash eventType, VariantMap& eventData)
{
}

}
