#pragma once

#include <Urho3D/Core/Object.h>

class SignalHandler : public Urho3D::Object
{
    URHO3D_OBJECT(SignalHandler, Urho3D::Object);

public:
    SignalHandler(Urho3D::Context* context);

private:
    void HandleUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
};
