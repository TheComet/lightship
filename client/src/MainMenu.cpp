#include "lightship-client/MainMenu.h"
#include "lightship-client/MenuScreen.h"
#include "lightship-client/ClientAPI.h"
#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/StringUtils.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/LineEdit.h>
#include <Urho3D/UI/Text.h>

using namespace Urho3D;

enum Buttons
{
#define X(name) name,
    BUTTONS_ALL
#undef X
};

static const char* g_buttonNames[] = {
#define X(name) #name,
    BUTTONS_ALL
#undef X
};

static const char* g_screenUIResources[] = {
    "UI/Menu_MainLocal.xml",
    "UI/Menu_MainServer.xml",
    "UI/Menu_Connect.xml",
    "UI/Menu_Connecting.xml",
    "UI/Menu_ConnectionFailed.xml"
};

template <class T>
T* GetUIChild(const UIElement* element, const Urho3D::String& name)
{
    UIElement* child = element->GetChild(name, true);
    if (child == NULL)
        return NULL;
    if (child->GetType() != T::GetTypeStatic())
        return NULL;
    return static_cast<T*>(child);
}

// ----------------------------------------------------------------------------
MainMenu::MainMenu(Context* context) :
    UIElement(context),
    client_(NULL),
    currentScreen_(SCREEN_MAINLOCAL)
{
    for (int i = 0; i != SCREEN_COUNT; ++i)
    {
        screens_[i] = new MenuScreen(context_);
        screens_[i]->LoadUI(g_screenUIResources[i]);
        screens_[i]->SetVisible(false);
        AddChild(screens_[i]);
    }

    SetHorizontalAlignment(HA_CENTER);
    SetVerticalAlignment(VA_CENTER);

    /*
     * Update our layout so the UI elements defined in the layout are able to
     * calculate their preferred sizes, then position ourselves in the center
     * of the screen.
     *
    UpdateLayout();
    for (int i = 0; i != SCREEN_COUNT; ++i)
    {
        const IntVector2& rootSize = GetSubsystem<UI>()->GetRoot()->GetSize();
        const IntVector2& elementSize = screens_[i]->GetSize();
        screens_[i]->SetPosition((rootSize - elementSize) / 2);
    }*/

    // Initial address to join
    {
        LineEdit* line = GetUIChild<LineEdit>(screens_[SCREEN_CONNECT], "ServerAddress");
        if (line != NULL)
            line->SetText("127.0.0.1:1337");
        else
            URHO3D_LOGERROR("Failed to get element \"ServerAddress\" from SCREEN_CONNECT UI");
    }

    SwitchToScreen(currentScreen_);

#define CONNECT_BUTTON(SCREEN, BTN) do {                                     \
            if (screens_[SCREEN]->ui_ == NULL) {                             \
                URHO3D_LOGERROR("Failed to load screen");                    \
                break;                                                       \
            }                                                                \
            UIElement* button = screens_[SCREEN]->ui_->GetChild(g_buttonNames[BTN], true); \
            if (button == NULL) {                                            \
                URHO3D_LOGERRORF("Couldn't find button \"%s\" in UI", g_buttonNames[BTN]); \
            }                                                                \
            SubscribeToEvent(button, E_CLICK, URHO3D_HANDLER(MainMenu, Handle_##BTN)); \
        } while(0);

#define X(name) CONNECT_BUTTON(SCREEN_MAINLOCAL, name)
    BUTTONS_MAINLOCAL
#undef X
#define X(name) CONNECT_BUTTON(SCREEN_MAINSERVER, name)
    BUTTONS_MAINSERVER
#undef X
#define X(name) CONNECT_BUTTON(SCREEN_CONNECT, name)
    BUTTONS_CONNECT
#undef X
#define X(name) CONNECT_BUTTON(SCREEN_CONNECTING, name)
    BUTTONS_CONNECTING
#undef X
#define X(name) CONNECT_BUTTON(SCREEN_CONNECTIONFAILED, name)
    BUTTONS_CONNECTIONFAILED
#undef X

#undef CONNECT_BUTTON
}

// ----------------------------------------------------------------------------
void MainMenu::RegisterObject(Urho3D::Context* context)
{
    context->RegisterFactory<MainMenu>("Lightship");
}

// ----------------------------------------------------------------------------
void MainMenu::SetClient(ClientAPI* client)
{
    client_ = client;
}

// ----------------------------------------------------------------------------
void MainMenu::SwitchToScreen(MainMenu::Screen screen)
{
    currentScreen_ = screen;
    for (int i = 0; i != SCREEN_COUNT; ++i)
        screens_[i]->SetVisible(false);

    screens_[screen]->SetVisible(true);
}

// ----------------------------------------------------------------------------
void MainMenu::PushScreen(MainMenu::Screen screen)
{
    screenStack_.Push(currentScreen_);
    SwitchToScreen(screen);
}

// ----------------------------------------------------------------------------
void MainMenu::PopScreen()
{
    if (screenStack_.Size() == 0)
        return;
    SwitchToScreen(screenStack_.Back());
    screenStack_.Pop();
}

// ----------------------------------------------------------------------------
void MainMenu::HandleKeyDown(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
}

// ----------------------------------------------------------------------------
void MainMenu::Handle_BTN_MAINLOCAL_CONNECT(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
    SwitchToScreen(SCREEN_CONNECT);
}
void MainMenu::Handle_BTN_MAINLOCAL_MAPEDITOR(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
}
void MainMenu::Handle_BTN_MAINLOCAL_OPTIONS(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
}
void MainMenu::Handle_BTN_MAINLOCAL_QUIT(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
}

// ----------------------------------------------------------------------------
void MainMenu::Handle_BTN_MAINSERVER_CREATEGAME(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
}
void MainMenu::Handle_BTN_MAINSERVER_JOINGAME(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
}
void MainMenu::Handle_BTN_MAINSERVER_DISCONNECT(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
}

// ----------------------------------------------------------------------------
void MainMenu::Handle_BTN_CONNECT_OK(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
    LineEdit* addressLine = GetUIChild<LineEdit>(screens_[SCREEN_CONNECT], "ServerAddress");
    if (addressLine == NULL)
    {
        URHO3D_LOGERROR("Failed to get LineEdit object \"ServerAddress\" from UI");
        return;
    }

    Vector<String> addressAndPort = addressLine->GetText().Split(':');
    String address = addressAndPort[0];
    unsigned int port = 1337;
    if (addressAndPort.Size() > 1)
        port = ToUInt(addressAndPort[1]);

    Text* connectMessage = GetUIChild<Text>(screens_[SCREEN_CONNECTING], "ConnectingMessage");
    if (connectMessage == NULL)
    {
        URHO3D_LOGERROR("Failed to get Text object \"ConnectingMessage\" from UI");
    }
    else
    {
        String msg;
        msg.AppendWithFormat("Connecting to %s on port %d...", address.CString(), port);
        connectMessage->SetText(msg);
    }

    SwitchToScreen(SCREEN_CONNECTING);
    client_->ConnectToServer(address, port);
}
void MainMenu::Handle_BTN_CONNECT_CANCEL(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
    SwitchToScreen(SCREEN_MAINLOCAL);
}

// ----------------------------------------------------------------------------
void MainMenu::Handle_BTN_CONNECTING_CANCEL(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
    client_->DisconnectFromServer();
    SwitchToScreen(SCREEN_MAINLOCAL);
}

// ----------------------------------------------------------------------------
void MainMenu::Handle_BTN_CONNECTIONFAILED_RETRY(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
    SwitchToScreen(SCREEN_CONNECT);
}
void MainMenu::Handle_BTN_CONNECTIONFAILED_CANCEL(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
    SwitchToScreen(SCREEN_MAINLOCAL);
}
