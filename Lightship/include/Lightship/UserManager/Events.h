#pragma once

#include <Urho3D/Core/Object.h>

/// Sent when a client connects (from the server's perspective) and is verified to use the server (username, password, etc.)
URHO3D_EVENT(E_CLIENTCONNECTEDANDVERIFIED, ClientConnectedAndVerified)
{
    URHO3D_PARAM(P_CONNECTION, Connection);  // Connection pointer
}

/// Sent when a server accepts and verifies the client (username, password, etc.)
URHO3D_EVENT(E_SERVERCONNECTEDANDVERIFIED, ServerConnectedAndVerified)
{
}
