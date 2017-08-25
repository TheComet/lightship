#pragma once

#include <Urho3D/Core/Object.h>

URHO3D_EVENT(E_CHATUSERLISTCHANGED, ChatUserListChanged)
{
    URHO3D_PARAM(P_USERLIST, UserList);  // StringVector, names of all users
}

URHO3D_EVENT(E_CHATUSERJOINED, ChatUserJoined)
{
    URHO3D_PARAM(P_USERNAME, Username);  // String, name of user who joined
}

URHO3D_EVENT(E_CHATUSERLEFT, ChatUserLeft)
{
    URHO3D_PARAM(P_USERNAME, Username);  // String, name of user who left
}
