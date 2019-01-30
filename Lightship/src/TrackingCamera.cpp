#include "Lightship/TrackingCamera.hpp"
#include <Urho3D/Scene/Node.h>

using namespace Urho3D;

namespace LS {

// ----------------------------------------------------------------------------
TrackingCamera::TrackingCamera(Context* context) :
    Object(context)
{

}

// ----------------------------------------------------------------------------
void TrackingCamera::SetNodes(Node* rotateNode, Node* cameraNode)
{
    rotateNode_ = rotateNode;
    cameraNode_ = cameraNode;
}

// ----------------------------------------------------------------------------
void TrackingCamera::SetTrackNode(Node* node)
{
    trackNode_ = node;
}

// ----------------------------------------------------------------------------
void TrackingCamera::Update(float timeStep)
{
    SharedPtr<Node> trackNode;
    SharedPtr<Node> rotateNode;
    SharedPtr<Node> cameraNode;
    if (
        (trackNode  = trackNode_.Lock())  == NULL ||
        (rotateNode = rotateNode_.Lock()) == NULL ||
        (cameraNode = cameraNode_.Lock()) == NULL
    )
        return;
}

}
