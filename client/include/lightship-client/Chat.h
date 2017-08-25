#pragma once

#include "lightship/ClientAPI/ChatAPI.h"
#include <Urho3D/UI/UIElement.h>

namespace Urho3D {
    class ListView;
    class LineEdit;
}

class Chat : public Urho3D::UIElement, public ChatAPI
{
    URHO3D_OBJECT(Chat, Urho3D::UIElement)

public:
    Chat(Urho3D::Context* context);
    static void RegisterObject(Urho3D::Context* context);

    void Initialise();
    void SetScope(unsigned char scope);
    unsigned char GetScope() const;

    Urho3D::String GetAndClearChatBoxMessageIfSelected();

    void AddMessage(const Urho3D::String& message);
    void RequestConnectedUsersList() override;

private:
    void HandleServerConnected(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleNetworkMessage(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleKeyDown(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

private:
    Urho3D::ListView* chatMessages_;
    Urho3D::LineEdit* chatBox_;
    Urho3D::StringVector connectedUsers_;
    unsigned char scope_;
};
