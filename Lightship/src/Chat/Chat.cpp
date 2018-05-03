#include "Lightship/Chat/Chat.h"

using namespace Urho3D;

// ----------------------------------------------------------------------------
Chat::Chat() :
    limit_(200)
{
}

// ----------------------------------------------------------------------------
int Chat::GetMessageLimit() const
{
    return limit_;
}

// ----------------------------------------------------------------------------
void Chat::SetMessageLimit(int limit)
{
    limit_ = limit;
    ClearMessages();
}
