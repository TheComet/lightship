#include "lightship/ChatView.h"
#include "lightship/ChatModel.h"
#include "lightship/ChatEvents.h"
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/UI/ListView.h>
#include <Urho3D/UI/LineEdit.h>
#include <Urho3D/UI/Text.h>

using namespace Urho3D;

// ----------------------------------------------------------------------------
ChatView::ChatView(Context* context) :
    UIElement(context)
{
}

// ----------------------------------------------------------------------------
void ChatView::Initialise()
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

    SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(ChatView, HandleKeyDown));
}

// ----------------------------------------------------------------------------
void ChatView::SetModel(ChatModel* model)
{
    if (model_)
        UnsubscribeFromEvent(model_, E_CHATNEWMESSAGE);

    model_ = model;
    if (model_ != NULL)
        SubscribeToEvent(model_, E_CHATNEWMESSAGE, URHO3D_HANDLER(ChatView, HandleNewMessage));
}

// ----------------------------------------------------------------------------
String ChatView::GetAndClearChatBoxMessageIfSelected()
{
    if (chatBox_->HasFocus() == false)
        return "";

    String msg = chatBox_->GetText();
    chatBox_->SetText("");
    return msg;
}

// ----------------------------------------------------------------------------
void ChatView::HandleKeyDown(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
    using namespace KeyDown;

    if (eventData[P_KEY].GetInt() == KEY_RETURN)
    {
        String chatMessage = GetAndClearChatBoxMessageIfSelected();
        if (model_ != NULL && chatMessage.Empty() == false)
        {
            model_->AddMessage(chatMessage);
        }
    }
}

// ----------------------------------------------------------------------------
void ChatView::HandleNewMessage(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
    using namespace ChatNewMessage;

    Text* text = new Text(context_);
    text->SetStyleAuto();
    text->SetText(eventData[P_MESSAGE].GetString());
    chatMessages_->AddItem(text);
    chatMessages_->EnsureItemVisibility(text);
    chatMessages_->UpdateLayout();
}
