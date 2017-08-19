#pragma once

#include <Urho3D/Core/StringUtils.h>

class ClientAPI
{
public:
    virtual void ConnectToServer(const Urho3D::String& address, unsigned int port) = 0;
    virtual void DisconnectFromServer() = 0;
};
