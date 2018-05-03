#pragma once

#include "Lightship/config.h"
#include <Urho3D/Core/Object.h>

namespace Urho3D {
    class Node;
}

class LIGHTSHIP_PUBLIC_API TrackingCamera : public Urho3D::Object
{
    URHO3D_OBJECT(TrackingCamera, Urho3D::Object);

public:
    TrackingCamera(Urho3D::Context* context);

    /*!
     * Assumes the camera is attached to a node (for controlling the distance
     * to a center position) which in turn is attached to a parent node for
     * controlling rotating around a central position.
     */
    void SetNodes(Urho3D::Node* rotateNode, Urho3D::Node* cameraNode);

    /// Sets the node to follow. Can be NULL.
    void SetTrackNode(Urho3D::Node* node);

    void Update(float timeStep);

private:
    Urho3D::WeakPtr<Urho3D::Node> rotateNode_;
    Urho3D::WeakPtr<Urho3D::Node> cameraNode_;
    Urho3D::WeakPtr<Urho3D::Node> trackNode_;
};
