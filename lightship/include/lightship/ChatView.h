#pragma once

#include <Urho3D/UI/UIElement.h>

namespace Urho3D {
    class ListView;
    class LineEdit;
}

class ChatModel;

class ChatView : public Urho3D::UIElement
{
    URHO3D_OBJECT(ChatView, Urho3D::UIElement)

public:
    ChatView(Urho3D::Context* context);

    void Initialise();
    void SetModel(ChatModel* model);

    Urho3D::String GetAndClearChatBoxMessageIfSelected();

private:
    void HandleKeyDown(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleNewMessage(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

private:
    Urho3D::SharedPtr<ChatModel> model_;
    Urho3D::ListView* chatMessages_;
    Urho3D::LineEdit* chatBox_;
};
