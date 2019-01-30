#include "Lightship/UserManager/User.hpp"

#include <Urho3D/IO/Serializer.h>
#include <Urho3D/IO/Deserializer.h>
#include <Urho3D/IO/Log.h>

using namespace Urho3D;

namespace LS {

// ----------------------------------------------------------------------------
User::User(Urho3D::Context* context, const String& username, int uid) :
    Serializable(context),
    username_(username),
    connection_(nullptr),
    uid_(uid)
{
}

// ----------------------------------------------------------------------------
const String& User::GetUsername() const
{
    return username_;
}

// ----------------------------------------------------------------------------
int User::GetUID() const
{
    return uid_;
}

// ----------------------------------------------------------------------------
void User::SetConnection(Connection* connection)
{
    connection_ = connection;
}

// ----------------------------------------------------------------------------
Connection * User::GetConnection() const
{
    return connection_;
}

// ----------------------------------------------------------------------------
bool User::Load(Deserializer& source)
{
    username_ = source.ReadString();
    uid_ = source.ReadInt();

    return Serializable::Load(source);
}

// ----------------------------------------------------------------------------
bool User::Save(Serializer& dest) const
{
    if (dest.WriteString(username_) == false)
    {
        URHO3D_LOGERROR("Could not save username, writing to stream failed");
        return false;
    }
    if (dest.WriteInt(uid_) == false)
    {
        URHO3D_LOGERROR("Could not save UID, writing to stream failed");
        return false;
    }

    return Serializable::Save(dest);
}

}
