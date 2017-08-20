#include "lightship/Chat.h"
#include "lightship/Protocol.h"
#include <Urho3D/Core/Context.h>
#include <Urho3D/Network/Network.h>
#include <Urho3D/Network/NetworkEvents.h>
#include <Urho3D/Network/Connection.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/IO/MemoryBuffer.h>
#include <Urho3D/IO/VectorBuffer.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/LineEdit.h>
#include <Urho3D/UI/ListView.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UIEvents.h>

#include <Urho3D/Resource/ResourceCache.h>

using namespace Urho3D;

// ----------------------------------------------------------------------------
Chat::Chat(Context* context) :
    UIElement(context)
{
}

// ----------------------------------------------------------------------------
void Chat::RegisterObject(Context* context)
{
    context->RegisterFactory<Chat>("Lightship");
}

// ----------------------------------------------------------------------------
void Chat::Initialise()
{
    chatMessages_ = new ListView(context_);
    chatMessages_->SetStyleAuto();
    AddChild(chatMessages_);

    chatBox_ = new LineEdit(context_);
    chatBox_->SetFixedHeight(20);
    chatBox_->SetStyleAuto();
    chatBox_->SetFocusMode(FM_FOCUSABLE);
    AddChild(chatBox_);

    SetLayoutMode(LM_VERTICAL);
    SetLayoutSpacing(5);
    UpdateLayout();

    SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(Chat, HandleKeyDown));
    SubscribeToEvent(E_NETWORKMESSAGE, URHO3D_HANDLER(Chat, HandleNetworkMessage));
}

// ----------------------------------------------------------------------------
void Chat::SetScope(unsigned char scope)
{
    scope_ = scope;
}

// ----------------------------------------------------------------------------
unsigned char Chat::GetScope() const
{
    return scope_;
}

// ----------------------------------------------------------------------------
String Chat::GetAndClearChatBoxMessageIfSelected()
{
    if (chatBox_->HasFocus() == false)
        return "";

    String msg = chatBox_->GetText();
    chatBox_->SetText("");
    return msg;
}

// ----------------------------------------------------------------------------
void Chat::HandleNetworkMessage(StringHash eventType, VariantMap& eventData)
{
    using namespace NetworkMessage;
    int messageID = eventData[P_MESSAGEID].GetInt();

    if (messageID == MSG_CHATMESSAGE)
    {
        MemoryBuffer buffer(eventData[P_DATA].GetBuffer());
        unsigned char messageType = buffer.ReadUByte();
        String message = buffer.ReadString();

        URHO3D_LOGDEBUGF("Received message \"%s\"", message.CString());

        if (messageType & GLOBAL)
        {
            Text* text = new Text(context_);
            text->SetStyleAuto();
            text->SetText(message);
            chatMessages_->AddItem(text);
            chatMessages_->EnsureItemVisibility(text);
            chatMessages_->UpdateLayout();
        }
    }
}

// ----------------------------------------------------------------------------
void Chat::HandleKeyDown(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
    using namespace KeyDown;

    if (eventData[P_KEY].GetInt() == KEY_RETURN)
    {
        String chatMessage = GetAndClearChatBoxMessageIfSelected();
        if (chatMessage.Empty() == false)
        {
            Connection* serverConnection = GetSubsystem<Network>()->GetServerConnection();
            if (serverConnection == NULL)
            {
                URHO3D_LOGERROR("Cannot send chat message: No server connection");
                return;
            }

            VectorBuffer buffer;
            buffer.WriteUByte(scope_);
            buffer.WriteString(chatMessage);
            serverConnection->SendMessage(MSG_CHATMESSAGE, true, false, buffer);
        }
    }
}
