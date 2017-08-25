#pragma once

#include <Urho3D/Core/Object.h>

URHO3D_EVENT(E_USERLISTCHANGED, UserListChanged)
{
    URHO3D_PARAM(P_USERLIST, UserList);  // StringVector, names of all users
}

URHO3D_EVENT(E_USERJOINED, UserJoined)
{
    URHO3D_PARAM(P_USERNAME, Username);  // String, name of user who joined
}

URHO3D_EVENT(E_USERLEFT, UserLeft)
{
    URHO3D_PARAM(P_USERNAME, Username);  // String, name of user who left
}
