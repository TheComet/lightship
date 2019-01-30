#include "Lightship/Menu/Menu.hpp"

#include <Urho3D/UI/LineEdit.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Network/Network.h>
#include <Urho3D/Network/NetworkEvents.h>

using namespace Urho3D;

namespace LS {

// ----------------------------------------------------------------------------
String MainMenu::CONNECTPROMPT_GetUsername() const
{
    LineEdit* usernameLine = GetUIChild<LineEdit>(screens_[SCREEN_CONNECTPROMPT], "Username");
    if (usernameLine == NULL)
    {
        URHO3D_LOGERROR("Failed to get LineEdit object \"Username\" from UI");
        return "";
    }

    return usernameLine->GetText();
}

// ----------------------------------------------------------------------------
void MainMenu::CONNECTPROMPT_SetUsername(const String& username)
{
    LineEdit* usernameLine = GetUIChild<LineEdit>(screens_[SCREEN_CONNECTPROMPT], "Username");
    if (usernameLine == NULL)
    {
        URHO3D_LOGERROR("Failed to get LineEdit object \"Username\" from UI");
        return;
    }

    usernameLine->SetText(username);
}

// ----------------------------------------------------------------------------
void MainMenu::CONNECTPROMPT_GetAddressAndPort(String* address, unsigned int* port) const
{
    *port = 1337;

    LineEdit* addressLine = GetUIChild<LineEdit>(screens_[SCREEN_CONNECTPROMPT], "ServerAddress");
    if (addressLine == NULL)
    {
        URHO3D_LOGERROR("Failed to get LineEdit object \"ServerAddress\" from UI");
        return;
    }

    Vector<String> addressAndPort = addressLine->GetText().Split(':');
    *address = addressAndPort[0];
    if (addressAndPort.Size() > 1)
        *port = ToUInt(addressAndPort[1]);
}

// ----------------------------------------------------------------------------
void MainMenu::Handle_BTN_CONNECTPROMPT_OK(StringHash eventType, VariantMap& eventData)
{
    String username = CONNECTPROMPT_GetUsername();
    if (username.Empty())
    {
        CONNECTPROMPT_SetUsername("Please enter a username");
        return;
    }

    String address;
    unsigned int port;
    CONNECTPROMPT_GetAddressAndPort(&address, &port);

    String msg;
    msg.AppendWithFormat("Connecting to %s on port %d...", address.CString(), port);
    Connecting_SetMessage(msg);

    EnableDisconnectedPrompt(true);

    // Scene is created later when the user creates or joins a game.
    VariantMap connectionData;
    connectionData["Username"] = username;
    if (GetSubsystem<Network>()->Connect(address, port, NULL, connectionData) == true)
        SwitchToScreen(SCREEN_CONNECTING);
    else
        SwitchToScreen(SCREEN_CONNECTIONFAILED);
}

// ----------------------------------------------------------------------------
void MainMenu::Handle_BTN_CONNECTPROMPT_CANCEL(StringHash eventType, VariantMap& eventData)
{
    EnableDisconnectedPrompt(false);
    GetSubsystem<Network>()->Disconnect();
    SwitchToScreen(SCREEN_MAINLOCAL);
}

}
