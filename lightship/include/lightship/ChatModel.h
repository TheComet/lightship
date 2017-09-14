#pragma once

#include <Urho3D/Core/Object.h>

class ChatModel : public Urho3D::Object
{
    URHO3D_OBJECT(ChatModel, Object)

public:
    enum Scope
    {
        GLOBAL = 0,
        LOBBY,
        IN_GAME
    };

    ChatModel(Urho3D::Context* context, Scope scope);

    Scope GetScope() const;

    int GetMessageLimit() const;
    void SetMessageLimit(int limit);

    Urho3D::StringVector GetMessages() const;
    Urho3D::String GetNewestMessage() const;
    virtual void AddMessage(const Urho3D::String& message);
    virtual void ClearMessages();

private:
    int Advance();

private:
    Scope scope_;
    int limit_;
    int insertIndex_;
    Urho3D::StringVector messages_;
};
