#pragma once

#include "lightship/ChatModel.h"

namespace Urho3D {
    class ListView;
    class LineEdit;
}

class ChatServerProtocol : public ChatModel
{
    URHO3D_OBJECT(ChatServerProtocol, ChatModel)

public:
    ChatServerProtocol(Urho3D::Context* context, Scope scope);

    virtual void AddMessage(const Urho3D::String& message) override;
    virtual void ClearMessages() override;

private:
    void HandleNetworkMessage(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
};
