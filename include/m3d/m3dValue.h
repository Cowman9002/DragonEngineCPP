#pragma once

/** ------------- typedef based controls
    sets how the library works, what types of floating points to use etc */

namespace m3d
{
    #ifdef M3D_DOUBLE
    typedef double value;
    #else
    typedef float value;
    #endif // M3D_DOUBLE
}
