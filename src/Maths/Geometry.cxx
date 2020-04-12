
export module Enigma.Maths.Geometry;

export namespace Enigma::Maths {
    template<int Dim = 2, typename Type = float> struct Point { };
    template<typename Type> struct Point<2, Type> { Type x, y; };
    template<typename Type> struct Point<3, Type> { Type x, y, z; };
    template<typename Type> struct Point<4, Type> { Type x, y, z, w; };

    template<int Dim = 2, typename Type = float> struct LineSegment { Point<Dim, Type> start, end; };
}
