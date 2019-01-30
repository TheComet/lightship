#pragma once

#include <Urho3D/Scene/Serializable.h>

namespace LS {

class User : public Urho3D::Serializable
{
    URHO3D_OBJECT(User, Urho3D::Serializable)
public:
    User(Urho3D::Context* context, const Urho3D::String& username, int uid);

    const Urho3D::String& GetUsername() const;
    int GetUID() const;

    /// Enables the User to be found via UserManager::GetUserByConnection()
    void SetConnection(Urho3D::Connection* connection);
    Urho3D::Connection* GetConnection() const;

    virtual bool Load(Urho3D::Deserializer& source) override;
    virtual bool Save(Urho3D::Serializer& dest) const override;

private:
    Urho3D::String username_;
    Urho3D::Connection* connection_;
    int uid_;
};

}
