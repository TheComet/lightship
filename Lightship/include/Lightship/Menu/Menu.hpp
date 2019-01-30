#pragma once

#include "Lightship/config.hpp"
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
#define BUTTONS_CONNECTPROMPT        \
    X(BTN_CONNECTPROMPT_OK)          \
    X(BTN_CONNECTPROMPT_CANCEL)
#define BUTTONS_CONNECTING           \
    X(BTN_CONNECTING_CANCEL)
#define BUTTONS_CONNECTIONFAILED     \
    X(BTN_CONNECTIONFAILED_RETRY)    \
    X(BTN_CONNECTIONFAILED_CANCEL)

#define BUTTONS_LIST                 \
    BUTTONS_MAINLOCAL                \
    BUTTONS_MAINSERVER               \
    BUTTONS_CONNECTPROMPT            \
    BUTTONS_CONNECTING               \
    BUTTONS_CONNECTIONFAILED

#define SCREENS_LIST                 \
    X(SCREEN_MAINLOCAL)              \
    X(SCREEN_MAINSERVER)             \
    X(SCREEN_CONNECTPROMPT)          \
    X(SCREEN_CONNECTING)             \
    X(SCREEN_CONNECTIONFAILED)

namespace LS {

class MenuScreen;

/*!
 * Helper function for getting a child UI element in a typesafe way.
 */
template <class T>
T* GetUIChild(const Urho3D::UIElement* element, const Urho3D::String& name)
{
    Urho3D::UIElement* child = element->GetChild(name, true);
    if (child == NULL)
        return NULL;
    if (child->GetType() != T::GetTypeStatic())
        return NULL;
    return static_cast<T*>(child);
}

class LIGHTSHIP_PUBLIC_API MainMenu : public Urho3D::UIElement
{
    URHO3D_OBJECT(MainMenu, Urho3D::UIElement)

public:
    enum Screen
    {
#define X(name) name,
        SCREENS_LIST
#undef X
        SCREEN_COUNT
    };

    MainMenu(Urho3D::Context* context);
    static void RegisterObject(Urho3D::Context* context);

    /*!
     * @brief Hides all UI elements of the currently active screen and shows
     * all UI elements of the screen specified.
     */
    void SwitchToScreen(Screen screen);

    /*!
     * @brief Stores the currently active screen and switches to the screen
     * specified. Calling PopScreen() will restore the previously pushed
     * screen. This is useful for navigating a tree of menus.
     */
    void PushScreen(Screen screen);

    /*!
     * @brief Switches to the screen that was last pushed using PushScreen().
     * If there are no screens on the stack then this method does nothing.
     */
    void PopScreen();

    /*!
     * @brief If enabled, switch to the "we got disconnected!" screen when we
     * get disconnected from the server.
     *
     * This is enabled when the user clicks on the "connect" button and
     * Network::Connect() is called so the user is informed when something with
     * the connection goes wrong.
     *
     * It is disabled when the client explicitly disconnects, for example by
     * clicking on "disconnect" in the main server screen or aborting the
     * connection process.
     */
    void EnableDisconnectedPrompt(bool enable);
private:

    // helpers for interacting with UI elements
    void CONNECTPROMPT_SetUsername(const Urho3D::String& username);
    Urho3D::String CONNECTPROMPT_GetUsername() const;
    void CONNECTPROMPT_GetAddressAndPort(Urho3D::String* address, unsigned int* port) const;
    void Connecting_SetMessage(const Urho3D::String& msg);
    void ConnectionFailed_SetMessage(const Urho3D::String& msg);

    // other events of interest
    void HandleChatUserListChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleChatUserJoined(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleChatUserLeft(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleConnectFailed(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleServerConnectedAndVerified(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleServerDisonnected(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

    // declares all event handlers for all buttons
#define X(name) void Handle_##name(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    BUTTONS_LIST
#undef X

    Urho3D::SharedPtr<Urho3D::UIElement> screens_[SCREEN_COUNT];
    Urho3D::PODVector<Screen> screenStack_;
    Screen currentScreen_;
};

}
