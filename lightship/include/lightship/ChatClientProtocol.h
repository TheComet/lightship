#pragma once

#include "lightship/ChatModel.h"

namespace Urho3D {
    class ListView;
    class LineEdit;
}

class ChatClientProtocol : public ChatModel
{
    URHO3D_OBJECT(ChatClientProtocol, ChatModel)

public:
    ChatClientProtocol(Urho3D::Context* context, Scope scope);

    void RequestConnectedUsersList();
    virtual void AddMessage(const Urho3D::String& message) override;
    virtual void ClearMessages() override;

private:
    void HandleNetworkMessage(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
};
