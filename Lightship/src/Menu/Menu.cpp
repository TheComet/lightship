#include "Lightship/Menu/Menu.hpp"
#include "Lightship/Chat/ChatClient.hpp"
#include "Lightship/Chat/Events.hpp"
#include "Lightship/UserManager/User.hpp"
#include "Lightship/UserManager/Events.hpp"
#include "Lightship/UserManager/ClientUserManager.hpp"

#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/StringUtils.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/IO/MemoryBuffer.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/Network/Network.h>
#include <Urho3D/Network/NetworkEvents.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/LineEdit.h>
#include <Urho3D/UI/ListView.h>
#include <Urho3D/UI/Text.h>

#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Graphics/Octree.h>

using namespace Urho3D;

namespace LS {

enum Buttons
{
#define X(name) name,
    BUTTONS_LIST
#undef X
    BUTTON_COUNT
};

static const char* g_buttonNames[] = {
#define X(name) #name,
    BUTTONS_LIST
#undef X
};

static const char* g_screenUIResources[] = {
#define X(name) "UI/" #name ".xml",
    SCREENS_LIST
#undef X
};

// ----------------------------------------------------------------------------
MainMenu::MainMenu(Context* context) :
    UIElement(context),
    currentScreen_(SCREEN_MAINLOCAL)
{
    SetAlignment(HA_CENTER, VA_CENTER);

    ResourceCache* cache = GetSubsystem<ResourceCache>();
    XMLFile* xmlDefaultStyle = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");
    for (int i = 0; i != SCREEN_COUNT; ++i)
    {
        screens_[i] = new UIElement(context_);
        XMLFile* xml = cache->GetResource<XMLFile>(g_screenUIResources[i]);
        if (xml)
        {
            screens_[i]->LoadXML(xml->GetRoot(), xmlDefaultStyle);
            screens_[i]->SetStyleAuto();
            screens_[i]->SetAlignment(HA_CENTER, VA_CENTER);
            screens_[i]->SetVisible(false);
        }
        AddChild(screens_[i]);
    }

    // Initial address to join
    {
        LineEdit* line = GetUIChild<LineEdit>(screens_[SCREEN_CONNECTPROMPT], "ServerAddress");
        if (line != nullptr)
            line->SetText("127.0.0.1:2048");
        else
            URHO3D_LOGERROR("Failed to get element \"ServerAddress\" from SCREEN_CONNECT UI");
    }

    // Add our custom chat box into the reserved UI element
    {
        UIElement* chatLayout = GetUIChild<UIElement>(screens_[SCREEN_MAINSERVER], "ChatMessages");
        if (chatLayout == nullptr)
        {
            URHO3D_LOGERROR("Failed to get UIElement object \"ChatMessages\" from UI");
            return;
        }
        ChatClient* chat = chatLayout->CreateChild<ChatClient>();
        chat->Initialise();
        chat->SetStyleAuto();
    }

    {
        UIElement* userListLayout = GetUIChild<UIElement>(screens_[SCREEN_MAINSERVER], "UserList");
        if (userListLayout == nullptr)
        {
            URHO3D_LOGERROR("Failed to get UIElement object \"UserList\" from UI");
            return;
        }
        userListLayout->CreateChild<ClientUserManager>();
    }

    SwitchToScreen(currentScreen_);

#define CONNECT_BUTTON(SCREEN, BTN) do {                                     \
            UIElement* button = screens_[SCREEN]->GetChild(g_buttonNames[BTN], true); \
            if (button == nullptr) {                                            \
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
#define X(name) CONNECT_BUTTON(SCREEN_CONNECTPROMPT, name)
    BUTTONS_CONNECTPROMPT
#undef X
#define X(name) CONNECT_BUTTON(SCREEN_CONNECTING, name)
    BUTTONS_CONNECTING
#undef X
#define X(name) CONNECT_BUTTON(SCREEN_CONNECTIONFAILED, name)
    BUTTONS_CONNECTIONFAILED
#undef X

#undef CONNECT_BUTTON

    SubscribeToEvent(E_CONNECTFAILED, URHO3D_HANDLER(MainMenu, HandleConnectFailed));
    SubscribeToEvent(E_SERVERCONNECTEDANDVERIFIED, URHO3D_HANDLER(MainMenu, HandleServerConnectedAndVerified));
}

// ----------------------------------------------------------------------------
void MainMenu::RegisterObject(Context* context)
{
    context->RegisterFactory<MainMenu>("Lightship");
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
void MainMenu::EnableDisconnectedPrompt(bool enable)
{
    if (enable)
        SubscribeToEvent(E_SERVERDISCONNECTED, URHO3D_HANDLER(MainMenu, HandleServerDisonnected));
    else
        UnsubscribeFromEvent(E_SERVERDISCONNECTED);
}

// ----------------------------------------------------------------------------
void MainMenu::HandleChatUserListChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{/*
    using namespace UserListChanged;

    ListView* userList = GetUIChild<ListView>(screens_[SCREEN_MAINSERVER], "ConnectedPlayers");
    if (userList == nullptr)
    {
        URHO3D_LOGERROR("Failed to get ListView object \"ConnectedPlayers\" from UI");
        return;
    }

    userList->RemoveAllItems();
    userList->DisableLayoutUpdate();
    StringVector* list = eventData[P_USERLIST].GetStringVectorPtr();
    for (StringVector::ConstIterator it = list->Begin(); it != list->End(); ++it)
    {
        Text* text = new Text(context_);
        text->SetStyleAuto();
        text->SetText(*it);
        userList->AddItem(text);
    }
    userList->EnableLayoutUpdate();
    userList->UpdateLayout();*/
}

// ----------------------------------------------------------------------------
void MainMenu::HandleChatUserJoined(StringHash eventType, VariantMap& eventData)
{/*
    using namespace UserJoined;

    ListView* userList = GetUIChild<ListView>(screens_[SCREEN_MAINSERVER], "ConnectedPlayers");
    if (userList == nullptr)
    {
        URHO3D_LOGERROR("Failed to get ListView object \"ConnectedPlayers\" from UI");
        return;
    }

    Text* text = new Text(context_);
    text->SetStyleAuto();
    text->SetText(eventData[P_USERNAME].GetString());
    userList->AddItem(text);
    userList->UpdateLayout();*/
}

// ----------------------------------------------------------------------------
void MainMenu::HandleChatUserLeft(StringHash eventType, VariantMap& eventData)
{/*
    using namespace UserLeft;

    ListView* userList = GetUIChild<ListView>(screens_[SCREEN_MAINSERVER], "ConnectedPlayers");
    if (userList == nullptr)
    {
        URHO3D_LOGERROR("Failed to get ListView object \"ConnectedPlayers\" from UI");
        return;
    }

    String username = eventData[P_USERNAME].GetString();
    PODVector<UIElement*> items = userList->GetItems();
    for (PODVector<UIElement*>::Iterator it = items.Begin(); it != items.End(); ++it)
    {
        Text* textItem = static_cast<Text*>(*it);
        if (textItem->GetText() == username)
        {
            userList->RemoveItem(textItem);
            userList->UpdateLayout();
            break;
        }
    }*/
}

// ----------------------------------------------------------------------------
void MainMenu::HandleConnectFailed(StringHash eventType, VariantMap& eventData)
{
    SwitchToScreen(SCREEN_CONNECTIONFAILED);
}

// ----------------------------------------------------------------------------
void MainMenu::HandleServerConnectedAndVerified(StringHash eventType, VariantMap& eventData)
{
    SwitchToScreen(SCREEN_MAINSERVER);
}

// ----------------------------------------------------------------------------
void MainMenu::HandleServerDisonnected(StringHash eventType, VariantMap& eventData)
{
    SwitchToScreen(SCREEN_CONNECTIONFAILED);
}

}
