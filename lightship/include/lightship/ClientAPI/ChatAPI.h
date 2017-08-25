#pragma once

#include <Urho3D/Container/Str.h>

class ChatAPI
{
public:
    virtual void RequestConnectedUsersList() = 0;
    virtual void AddMessage(const Urho3D::String& message) = 0;
};
