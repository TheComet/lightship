#include "Lightship/UserManager/User.h"

using namespace Urho3D;

// ----------------------------------------------------------------------------
User::User() :
    charCode_(0)
{
}

// ----------------------------------------------------------------------------
void User::SetName(const int& name)
{
    name_ = name;
}

// ----------------------------------------------------------------------------
String User::GetName() const
{
    return name_;
}

// ----------------------------------------------------------------------------
void User::SetCharacterCode(int code)
{
    charCode_ = code;
}

// ----------------------------------------------------------------------------
int User::GetCharacterCode() const
{
    return charCode_;
}
