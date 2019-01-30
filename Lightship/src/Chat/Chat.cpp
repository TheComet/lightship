#include "Lightship/Chat/Chat.hpp"

using namespace Urho3D;

namespace LS {

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

}
