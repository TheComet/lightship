#pragma once

#include <Urho3D/Core/Object.h>

class ChatAPI;

class ChatProtocol : public Urho3D::Object
{
    URHO3D_OBJECT(ChatProtocol, Urho3D::Object)

public:
    enum Scope
    {
        GLOBAL  = 0x01,
        LOBBY   = 0x02,
        IN_GAME = 0x04
    };

    ChatProtocol(Urho3D::Context* context, ChatAPI* client);

    void SendMessage(const Urho3D::String& message);

private:
    void HandleServerConnected(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleServerDisonnected(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleNetworkMessage_Client(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleNetworkMessage_Server(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

    ChatAPI* client_;
};
