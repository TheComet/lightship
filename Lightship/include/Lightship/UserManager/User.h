#pragma once

#include <Urho3D/Core/StringUtil.h>

class User
{
public:
    User();

    void SetName(const Urho3D::String& name);
    Urho3D::String GetName() const;

    void SetCharacterCode(int code);
    int GetCharacterCode() const;

private:
    Urho3D::String name_;
    int charCode_;
};
