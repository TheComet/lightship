#include "Lightship/Chat/ChatClient.h"
#include "Lightship/Network/Protocol.h"
#include <Urho3D/Core/Context.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/IO/VectorBuffer.h>
#include <Urho3D/IO/MemoryBuffer.h>
#include <Urho3D/Network/Network.h>
#include <Urho3D/Network/NetworkEvents.h>
#include <Urho3D/UI/ListView.h>
#include <Urho3D/UI/LineEdit.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/IO/Log.h>

using namespace Urho3D;

// ----------------------------------------------------------------------------
ChatClient::ChatClient(Context* context) :
    UIElement(context)
{
    SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(ChatClient, HandleKeyDown));
    SubscribeToEvent(E_SERVERCONNECTED, URHO3D_HANDLER(ChatClient, HandleServerConnected));
    SubscribeToEvent(E_SERVERDISCONNECTED, URHO3D_HANDLER(ChatClient, HandleServerDisconnected));
    SubscribeToEvent(E_NETWORKMESSAGE, URHO3D_HANDLER(ChatClient, HandleNetworkMessage));
}

// ----------------------------------------------------------------------------
void ChatClient::RegisterObject(Urho3D::Context* context)
{
    context->RegisterFactory<ChatClient>("Lightship");
}

// ----------------------------------------------------------------------------
void ChatClient::Initialise()
{
    chatMessages_ = new ListView(context_);
    chatMessages_->SetStyleAuto();
    AddChild(chatMessages_);

    inputBox_ = new LineEdit(context_);
    inputBox_->SetFixedHeight(20);
    inputBox_->SetStyleAuto();
    inputBox_->SetFocusMode(FM_FOCUSABLE);
    AddChild(inputBox_);

    SetLayoutMode(LM_VERTICAL);
    SetLayoutSpacing(5);
    UpdateLayout();
}

// ----------------------------------------------------------------------------
void ChatClient::SetConnected()
{
    AddMessage(">>> Connected", Color::RED);
    inputBox_->SetEnabled(true);
}

// ----------------------------------------------------------------------------
void ChatClient::SetDisconnected()
{
    AddMessage("<<< Disconnected", Color::RED);
    inputBox_->SetEnabled(false);
}

// ----------------------------------------------------------------------------
String ChatClient::GetAndClearInputBoxIfSelected()
{
    if (inputBox_->HasFocus() == false)
        return "";

    String msg = inputBox_->GetText();
    inputBox_->SetText("");
    return msg;
}

// ----------------------------------------------------------------------------
StringVector ChatClient::GetMessages() const
{
    return StringVector();
}

// ----------------------------------------------------------------------------
String ChatClient::GetNewestMessage() const
{
    return "";
}

// ----------------------------------------------------------------------------
void ChatClient::AddMessage(const String& message, const Color& color)
{
    Text* text = new Text(context_);
    text->SetStyleAuto();
    text->SetText(message);
    text->SetColor(color);
    chatMessages_->AddItem(text);
    chatMessages_->EnsureItemVisibility(text);

    if (chatMessages_->GetNumItems() > GetMessageLimit())
    {
        chatMessages_->RemoveItem(0u);
    }

    //chatMessages_->UpdateLayout();
}

// ----------------------------------------------------------------------------
void ChatClient::ClearMessages()
{
    chatMessages_->RemoveAllItems();
}

// ----------------------------------------------------------------------------
void ChatClient::HandleKeyDown(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
    using namespace KeyDown;

    int key = eventData[P_KEY].GetInt();
    if (key != KEY_RETURN)
        return;

    String chatMessage = GetAndClearInputBoxIfSelected();
    if (chatMessage.Empty())
        return;

    VectorBuffer buffer;
    Connection* serverConnection = GetSubsystem<Network>()->GetServerConnection();
    if (serverConnection == NULL)
        return;

    buffer.WriteString(chatMessage);
    serverConnection->SendMessage(MSG_CHAT_MESSAGE, true, false, buffer);
}

// ----------------------------------------------------------------------------
void ChatClient::HandleServerConnected(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
    Connection* serverConnection = GetSubsystem<Network>()->GetServerConnection();
    serverConnection->SendMessage(MSG_CHAT_REQUEST_HISTORY, true, false, VectorBuffer());

    SetConnected();
}

// ----------------------------------------------------------------------------
void ChatClient::HandleServerDisconnected(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
    SetDisconnected();
}

// ----------------------------------------------------------------------------
void ChatClient::HandleNetworkMessage(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
    using namespace NetworkMessage;

    int messageID = eventData[P_MESSAGEID].GetInt();
    if (messageID == MSG_CHAT_MESSAGE)
    {
        MemoryBuffer buffer(eventData[P_DATA].GetBuffer());
        String chatMessage = buffer.ReadString();

        AddMessage(chatMessage, Color::WHITE);
    }

    if (messageID == MSG_CHAT_REQUEST_HISTORY)
    {
        MemoryBuffer buffer(eventData[P_DATA].GetBuffer());
        int messageCount = buffer.ReadInt();
        URHO3D_LOGDEBUGF("Message history has %d messages.", messageCount);
        for (unsigned i = 0; i < messageCount; ++i)
        {
            String chatMessage = buffer.ReadString();
            AddMessage(chatMessage, Color::WHITE);
        }
    }
}
