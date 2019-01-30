#include "Lightship/Menu/Menu.hpp"

#include <Urho3D/UI/Text.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Network/Network.h>
#include <Urho3D/Network/NetworkEvents.h>

using namespace Urho3D;

namespace LS {

// ----------------------------------------------------------------------------
void MainMenu::Connecting_SetMessage(const String& msg)
{
    Text* textElement = GetUIChild<Text>(screens_[SCREEN_CONNECTING], "ConnectingMessage");
    if (textElement == NULL)
    {
        URHO3D_LOGERROR("Failed to get Text object \"ConnectingMessage\" from UI");
        return;
    }

    textElement->SetText(msg);
}

// ----------------------------------------------------------------------------
void MainMenu::Handle_BTN_CONNECTING_CANCEL(StringHash eventType, VariantMap& eventData)
{
    UnsubscribeFromEvent(E_SERVERDISCONNECTED);
    GetSubsystem<Network>()->Disconnect();
    SwitchToScreen(SCREEN_MAINLOCAL);
}

}
