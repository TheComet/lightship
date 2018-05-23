#pragma once

#include "Lightship/Chat/Chat.h"
#include <Urho3D/UI/UIElement.h>

namespace Urho3D {
    class ListView;
    class LineEdit;
    class Text;
}

class ChatClient : public Chat,
                   public Urho3D::UIElement
{
    URHO3D_OBJECT(ChatClient, Urho3D::UIElement)
public:
    ChatClient(Urho3D::Context* context);
    static void RegisterObject(Urho3D::Context* context);

    /// Needs to be called after adding the chat view to the UI hierarchy
    void Initialise();
    void SetDisconnected();
    void SetConnected();
    Urho3D::String GetAndClearInputBoxIfSelected();

    // Overrides from ChatBase
    virtual Urho3D::StringVector GetMessages() const override;
    virtual Urho3D::String GetNewestMessage() const override;
    virtual void AddMessage(const Urho3D::String& message, const Urho3D::Color& color=Urho3D::Color::WHITE) override;
    virtual void ClearMessages() override;

private:
    void HandleKeyDown(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleServerConnectedAndVerified(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleServerDisconnected(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleNetworkMessage(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

private:
    Urho3D::ListView* chatMessages_;
    Urho3D::LineEdit* inputBox_;
};
