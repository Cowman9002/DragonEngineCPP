#pragma once

#define PI 3.1415926535897
#define TO_RADS (PI / 180.0)
#define TO_DEGS (180.0 / PI)

namespace m3d
{
    float clamp(const float& v, const float& low, const float& high);

    float lerp(const float& a, const float& b, const float& t);
}
