#include "Lightship/Menu/Menu.hpp"

#include <Urho3D/UI/Text.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Network/Network.h>

using namespace Urho3D;

namespace LS {

// ----------------------------------------------------------------------------
void MainMenu::Handle_BTN_MAINSERVER_CREATEGAME(StringHash eventType, VariantMap& eventData)
{
}
void MainMenu::Handle_BTN_MAINSERVER_JOINGAME(StringHash eventType, VariantMap& eventData)
{
}
void MainMenu::Handle_BTN_MAINSERVER_DISCONNECT(StringHash eventType, VariantMap& eventData)
{
    EnableDisconnectedPrompt(false);
    GetSubsystem<Network>()->Disconnect();
    SwitchToScreen(SCREEN_MAINLOCAL);
}

}
