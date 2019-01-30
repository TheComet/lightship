#pragma once

#include "Lightship/config.hpp"
#include "Lightship/Chat/Chat.hpp"
#include <Urho3D/Core/Object.h>

namespace LS {

class LIGHTSHIP_PUBLIC_API ChatServer :
    public Chat,
    public Urho3D::Object
{
    URHO3D_OBJECT(ChatServer, Urho3D::Object)
public:
    ChatServer(Urho3D::Context* context);

    // Overrides from ChatBase
    virtual Urho3D::StringVector GetMessages() const override;
    virtual Urho3D::String GetNewestMessage() const override;
    virtual void AddMessage(const Urho3D::String& message, const Urho3D::Color& color=Urho3D::Color::WHITE) override;
    virtual void ClearMessages() override;

private:
    // Advances ringbuffer pointer and returns it
    int Advance();
    void HandleNetworkMessage(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

private:
    int insertIndex_;
    Urho3D::StringVector messages_;
};

}
