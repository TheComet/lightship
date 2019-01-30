#include "Lightship/Menu/Menu.hpp"

#include <Urho3D/UI/Text.h>
#include <Urho3D/IO/Log.h>

using namespace Urho3D;

namespace LS {

// ----------------------------------------------------------------------------
void MainMenu::ConnectionFailed_SetMessage(const String& msg)
{
    Text* textElement = GetUIChild<Text>(screens_[SCREEN_CONNECTIONFAILED], "ErrorMessage");
    if (textElement == NULL)
    {
        URHO3D_LOGERROR("Failed to get Text object \"ErrorMessage\" from UI");
        return;
    }

    textElement->SetText(msg);
}

// ----------------------------------------------------------------------------
void MainMenu::Handle_BTN_CONNECTIONFAILED_RETRY(StringHash eventType, VariantMap& eventData)
{
    SwitchToScreen(SCREEN_CONNECTPROMPT);
}
void MainMenu::Handle_BTN_CONNECTIONFAILED_CANCEL(StringHash eventType, VariantMap& eventData)
{
    EnableDisconnectedPrompt(false);
    SwitchToScreen(SCREEN_MAINLOCAL);
}

}
