#pragma once

#include <Urho3D/UI/UIElement.h>

#define BUTTONS_MAINLOCAL            \
    X(BTN_MAINLOCAL_CONNECT)         \
    X(BTN_MAINLOCAL_MAPEDITOR)       \
    X(BTN_MAINLOCAL_OPTIONS)         \
    X(BTN_MAINLOCAL_QUIT)
#define BUTTONS_MAINSERVER           \
    X(BTN_MAINSERVER_CREATEGAME)     \
    X(BTN_MAINSERVER_JOINGAME)       \
    X(BTN_MAINSERVER_DISCONNECT)
#define BUTTONS_CONNECT              \
    X(BTN_CONNECT_OK)                \
    X(BTN_CONNECT_CANCEL)
#define BUTTONS_CONNECTING           \
    X(BTN_CONNECTING_CANCEL)
#define BUTTONS_CONNECTIONFAILED     \
    X(BTN_CONNECTIONFAILED_RETRY)    \
    X(BTN_CONNECTIONFAILED_CANCEL)

#define BUTTONS_ALL                  \
    BUTTONS_MAINLOCAL                \
    BUTTONS_MAINSERVER               \
    BUTTONS_CONNECT                  \
    BUTTONS_CONNECTING               \
    BUTTONS_CONNECTIONFAILED

class ClientAPI;
class MenuScreen;

class MainMenu : public Urho3D::UIElement
{
    URHO3D_OBJECT(MainMenu, Urho3D::UIElement)

public:
    enum Screen
    {
        SCREEN_MAINLOCAL,
        SCREEN_MAINSERVER,
        SCREEN_CONNECT,
        SCREEN_CONNECTING,
        SCREEN_CONNECTIONFAILED,

        SCREEN_COUNT
    };

    MainMenu(Urho3D::Context* context);
    static void RegisterObject(Urho3D::Context* context);

    void SetClient(ClientAPI* client);

    void SwitchToScreen(Screen screen);
    void PushScreen(Screen screen);
    void PopScreen();

private:
    // declares all screens handlers
#define X(name) void Handle_##name(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    BUTTONS_ALL
#undef X

    // input events
    void HandleKeyDown(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

    Urho3D::SharedPtr<MenuScreen> screens_[SCREEN_COUNT];
    Urho3D::PODVector<Screen> screenStack_;
    ClientAPI* client_;
    Screen currentScreen_;
};
