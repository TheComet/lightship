#pragma once

#include "Lightship/config.hpp"
#include <Urho3D/Math/Vector3.h>
#include <Urho3D/Math/Matrix3.h>

namespace LS {

class LIGHTSHIP_PUBLIC_API Math
{
public:

    /*!
     * @brief Wraps the specified angle so it is within the interval [-180 180]
     */
    static float Wrap180(float angle);
};

}
