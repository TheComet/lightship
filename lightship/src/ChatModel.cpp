#include "lightship/ChatModel.h"
#include "lightship/ChatEvents.h"

using namespace Urho3D;

// ----------------------------------------------------------------------------
ChatModel::ChatModel(Context* context, Scope scope) :
    Object(context),
    scope_(scope),
    limit_(100),
    insertIndex_(0)
{
}

// ----------------------------------------------------------------------------
ChatModel::Scope ChatModel::GetScope() const
{
    return scope_;
}

// ----------------------------------------------------------------------------
int ChatModel::GetMessageLimit() const
{
    return limit_;
}

// ----------------------------------------------------------------------------
void ChatModel::SetMessageLimit(int limit)
{
    limit_ = limit;
    ClearMessages();
}

// ----------------------------------------------------------------------------
Urho3D::StringVector ChatModel::GetMessages() const
{
    Urho3D::StringVector result;

    int iter = insertIndex_ - 1;
    if (iter < 0)
        iter = messages_.Size() - 1;
    if (iter < 0)
        return result;

    while (iter != insertIndex_)
    {
        result.Push(messages_[iter]);
        if (++iter > messages_.Size())
            iter = 0;
    }

    return result;
}

// ----------------------------------------------------------------------------
Urho3D::String ChatModel::GetNewestMessage() const
{
    int last = insertIndex_ - 1;
    if (last < 0)
        last = messages_.Size() - 1;
    if (last < 0)
        return "";

    return messages_[last];
}

// ----------------------------------------------------------------------------
void ChatModel::AddMessage(const String& message)
{
    if (messages_.Size() < limit_)
        messages_.Push(message);
    else
        messages_[Advance()] = message;

    VariantMap& eventData = GetEventDataMap();
    eventData[ChatNewMessage::P_MESSAGE] = message;
    SendEvent(E_CHATNEWMESSAGE, eventData);
}

// ----------------------------------------------------------------------------
void ChatModel::ClearMessages()
{
    messages_.Clear();
    insertIndex_ = 0;
}

// ----------------------------------------------------------------------------
int ChatModel::Advance()
{
    int insert = insertIndex_++;
    if (insertIndex_ >= limit_)
        insertIndex_ = 0;
    return insert;
}
