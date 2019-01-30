#pragma once

#include <Urho3D/Core/Object.h>

namespace LS {

/// New message is added
URHO3D_EVENT(E_CHATNEWMESSAGE, ChatNewMessage)
{
    URHO3D_PARAM(P_MESSAGE, Message);    // String
}

}
