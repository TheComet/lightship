#include "lightship-client/MainMenu.h"
#include "lightship-client/Chat.h"
#include "lightship/Network/UserManagerEvents.h"
#include "lightship/Network/ClientProtocol.h"
#include "lightship/Network/UserManager.h"
#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/StringUtils.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/IO/MemoryBuffer.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/Network/NetworkEvents.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/LineEdit.h>
#include <Urho3D/UI/ListView.h>
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
T* GetUIChild(const UIElement* element, const String& name)
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
    currentScreen_(SCREEN_MAINLOCAL)
{
}

// ----------------------------------------------------------------------------
void MainMenu::RegisterObject(Context* context)
{
    context->RegisterFactory<MainMenu>("Lightship");
}

// ----------------------------------------------------------------------------
void MainMenu::Initialise()
{
    UIElement* root = GetSubsystem<UI>()->GetRoot();
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    XMLFile* xmlDefaultStyle = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");

    for (int i = 0; i != SCREEN_COUNT; ++i)
    {
        XMLFile* xml = cache->GetResource<XMLFile>(g_screenUIResources[i]);
        screens_[i] = new UIElement(context_);
        screens_[i]->LoadXML(xml->GetRoot(), xmlDefaultStyle);
        screens_[i]->SetStyleAuto();
        screens_[i]->SetAlignment(HA_CENTER, VA_CENTER);
        screens_[i]->SetVisible(false);
        root->AddChild(screens_[i]);
    }

    // Initial address to join
    {
        LineEdit* line = GetUIChild<LineEdit>(screens_[SCREEN_CONNECT], "ServerAddress");
        if (line != NULL)
            line->SetText("127.0.0.1:2048");
        else
            URHO3D_LOGERROR("Failed to get element \"ServerAddress\" from SCREEN_CONNECT UI");
    }

    // Add our custom chat box into the reserved UI element
    {
        UIElement* chatLayout = GetUIChild<UIElement>(screens_[SCREEN_MAINSERVER], "ChatMessages");
        if (chatLayout == NULL)
        {
            URHO3D_LOGERROR("Failed to get UIElement object \"ChatMessages\" from UI");
            return;
        }
        Chat* chat = new Chat(context_);
        chat->SetStyleAuto();
        chatLayout->AddChild(chat);
        chat->Initialise();
    }

    SwitchToScreen(currentScreen_);

#define CONNECT_BUTTON(SCREEN, BTN) do {                                     \
            UIElement* button = screens_[SCREEN]->GetChild(g_buttonNames[BTN], true); \
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

    SubscribeToEvent(E_USERLISTCHANGED, URHO3D_HANDLER(MainMenu, HandleChatUserListChanged));
    SubscribeToEvent(E_USERJOINED, URHO3D_HANDLER(MainMenu, HandleChatUserJoined));
    SubscribeToEvent(E_USERLEFT, URHO3D_HANDLER(MainMenu, HandleChatUserLeft));
    SubscribeToEvent(E_CONNECTFAILED, URHO3D_HANDLER(MainMenu, HandleConnectFailed));
    SubscribeToEvent(E_SERVERCONNECTED, URHO3D_HANDLER(MainMenu, HandleServerConnected));
    SubscribeToEvent(E_SERVERDISCONNECTED, URHO3D_HANDLER(MainMenu, HandleServerDisonnected));
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
String MainMenu::Connect_GetUsername() const
{
    LineEdit* usernameLine = GetUIChild<LineEdit>(screens_[SCREEN_CONNECT], "Username");
    if (usernameLine == NULL)
    {
        URHO3D_LOGERROR("Failed to get LineEdit object \"Username\" from UI");
        return "";
    }

    return usernameLine->GetText();
}

// ----------------------------------------------------------------------------
void MainMenu::Connect_SetUsername(const String& username)
{
    LineEdit* usernameLine = GetUIChild<LineEdit>(screens_[SCREEN_CONNECT], "Username");
    if (usernameLine == NULL)
    {
        URHO3D_LOGERROR("Failed to get LineEdit object \"Username\" from UI");
        return;
    }

    usernameLine->SetText(username);
}

// ----------------------------------------------------------------------------
void MainMenu::Connect_GetAddressAndPort(String* address, unsigned int* port) const
{
    *port = 1337;

    LineEdit* addressLine = GetUIChild<LineEdit>(screens_[SCREEN_CONNECT], "ServerAddress");
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
void MainMenu::HandleChatUserListChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
    using namespace UserListChanged;

    ListView* userList = GetUIChild<ListView>(screens_[SCREEN_MAINSERVER], "ConnectedPlayers");
    if (userList == NULL)
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
    userList->UpdateLayout();
}

// ----------------------------------------------------------------------------
void MainMenu::HandleChatUserJoined(StringHash eventType, VariantMap& eventData)
{
    using namespace UserJoined;

    ListView* userList = GetUIChild<ListView>(screens_[SCREEN_MAINSERVER], "ConnectedPlayers");
    if (userList == NULL)
    {
        URHO3D_LOGERROR("Failed to get ListView object \"ConnectedPlayers\" from UI");
        return;
    }

    Text* text = new Text(context_);
    text->SetStyleAuto();
    text->SetText(eventData[P_USERNAME].GetString());
    userList->AddItem(text);
    userList->UpdateLayout();
}

// ----------------------------------------------------------------------------
void MainMenu::HandleChatUserLeft(StringHash eventType, VariantMap& eventData)
{
    using namespace UserLeft;

    ListView* userList = GetUIChild<ListView>(screens_[SCREEN_MAINSERVER], "ConnectedPlayers");
    if (userList == NULL)
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
    }
}

// ----------------------------------------------------------------------------
void MainMenu::HandleConnectFailed(StringHash eventType, VariantMap& eventData)
{
    SwitchToScreen(SCREEN_CONNECTIONFAILED);
}

// ----------------------------------------------------------------------------
void MainMenu::HandleServerConnected(StringHash eventType, VariantMap& eventData)
{
    SwitchToScreen(SCREEN_MAINSERVER);
}

// ----------------------------------------------------------------------------
void MainMenu::HandleServerDisonnected(StringHash eventType, VariantMap& eventData)
{
    SwitchToScreen(SCREEN_CONNECTIONFAILED);
}

// ----------------------------------------------------------------------------
void MainMenu::Handle_BTN_MAINLOCAL_CONNECT(StringHash eventType, VariantMap& eventData)
{
    SwitchToScreen(SCREEN_CONNECT);
}
void MainMenu::Handle_BTN_MAINLOCAL_MAPEDITOR(StringHash eventType, VariantMap& eventData)
{
}
void MainMenu::Handle_BTN_MAINLOCAL_OPTIONS(StringHash eventType, VariantMap& eventData)
{
}
void MainMenu::Handle_BTN_MAINLOCAL_QUIT(StringHash eventType, VariantMap& eventData)
{
    GetSubsystem<ClientProtocol>()->Quit();
}

// ----------------------------------------------------------------------------
void MainMenu::Handle_BTN_MAINSERVER_CREATEGAME(StringHash eventType, VariantMap& eventData)
{
}
void MainMenu::Handle_BTN_MAINSERVER_JOINGAME(StringHash eventType, VariantMap& eventData)
{
}
void MainMenu::Handle_BTN_MAINSERVER_DISCONNECT(StringHash eventType, VariantMap& eventData)
{
    GetSubsystem<ClientProtocol>()->DisconnectFromServer();
    SwitchToScreen(SCREEN_MAINLOCAL);
}

// ----------------------------------------------------------------------------
void MainMenu::Handle_BTN_CONNECT_OK(StringHash eventType, VariantMap& eventData)
{
    String username = Connect_GetUsername();
    if (username.Empty())
    {
        Connect_SetUsername("Please enter a username");
        return;
    }

    String address;
    unsigned int port;
    Connect_GetAddressAndPort(&address, &port);

    String msg;
    msg.AppendWithFormat("Connecting to %s on port %d...", address.CString(), port);
    Connecting_SetMessage(msg);

    if (GetSubsystem<ClientProtocol>()->ConnectToServer(address, port) == true)
        SwitchToScreen(SCREEN_CONNECTING);
    else
        SwitchToScreen(SCREEN_CONNECTIONFAILED);
}
void MainMenu::Handle_BTN_CONNECT_CANCEL(StringHash eventType, VariantMap& eventData)
{
    GetSubsystem<ClientProtocol>()->AbortConnectingToServer();
    SwitchToScreen(SCREEN_MAINLOCAL);
}

// ----------------------------------------------------------------------------
void MainMenu::Handle_BTN_CONNECTING_CANCEL(StringHash eventType, VariantMap& eventData)
{
    GetSubsystem<ClientProtocol>()->DisconnectFromServer();
    SwitchToScreen(SCREEN_MAINLOCAL);
}

// ----------------------------------------------------------------------------
void MainMenu::Handle_BTN_CONNECTIONFAILED_RETRY(StringHash eventType, VariantMap& eventData)
{
    SwitchToScreen(SCREEN_CONNECT);
}
void MainMenu::Handle_BTN_CONNECTIONFAILED_CANCEL(StringHash eventType, VariantMap& eventData)
{
    SwitchToScreen(SCREEN_MAINLOCAL);
}
