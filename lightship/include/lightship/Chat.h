#pragma once

#include "lightship/config.h"
#include <Urho3D/UI/UIElement.h>

namespace Urho3D {
    class ListView;
    class LineEdit;
}

class LIGHTSHIP_PUBLIC_API Chat : public Urho3D::UIElement
{
    URHO3D_OBJECT(Chat, Urho3D::UIElement)

public:
    enum Scope
    {
        GLOBAL  = 0x01,
        LOBBY   = 0x02,
        IN_GAME = 0x04
    };

    enum NetworkMessageAction
    {
        REQUEST_CONNECTED_USERS = 0,
        RECEIVE_CONNECTED_USERS,
        RECEIVE_JOINED_USER,
        RECEIVE_LEFT_USER
    };

    Chat(Urho3D::Context* context);
    static void RegisterObject(Urho3D::Context* context);

    void Initialise();
    void SetScope(unsigned char scope);
    unsigned char GetScope() const;
    void AddChatMessage(const Urho3D::String& message);

    Urho3D::String GetAndClearChatBoxMessageIfSelected();

private:
    void RequestConnectedUsersList() const;
    void HandleServerConnected(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleNetworkMessage(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleKeyDown(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

private:
    Urho3D::ListView* chatMessages_;
    Urho3D::LineEdit* chatBox_;
    Urho3D::StringVector connectedUsers_;
    unsigned char scope_;
};
