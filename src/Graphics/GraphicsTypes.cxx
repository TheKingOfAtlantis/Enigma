
#if defined(_WIN32)
#include <Windows.h>
#endif

export module Enigma.Graphics.Types;

import Enigma.Maths.Geometry;

namespace Enigma::Graphics {
    export template<typename Type = int> using Position = Maths::Point<2, Type>;
    export template<typename Type = int> struct Dimensions { Type width, height; };
    export template<typename Type = int> struct Boundary {
        Type left, top, right, bottom;
    #if defined(_WIN32)
        operator tagRECT() { return { (LONG)left, (LONG)top, (LONG)right, (LONG)bottom }; }
    #endif
    };
}
