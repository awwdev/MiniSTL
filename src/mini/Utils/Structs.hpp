#pragma once

#include "mini/Math/Matrix.hpp"
#include <iostream>

namespace mini::utils
{
    using NormColor4f = math::Vec4f;
    using RGBAColor4u = math::Vec<u8, 4>;

    struct Common_Vertex //could be used by physics too, so not rendering only?
    {
        alignas(16) math::Vec3f pos;
        alignas(16) math::Vec3f nor;
        alignas(16) math::Vec4f col;
        alignas(16) math::Vec2f tex;
    };

    std::ostream& operator<<(std::ostream& os, const Common_Vertex& vert)
    {
        PrintMatrix(vert.pos);
        PrintMatrix(vert.nor);
        PrintMatrix(vert.col);
        PrintMatrix(vert.tex);
        return os;
    }

    inline NormColor4f NormaliseColor(const RGBAColor4u& col) { 
        using namespace math;
        return { //normalise on gpu always instead??
            col[X] / 255.f, 
            col[Y] / 255.f, 
            col[Z] / 255.f, 
            col[W] / 255.f 
        };
    }

    inline RGBAColor4u HighlightColor(const RGBAColor4u& col, const u8 amount)
    {
        using namespace math;
        RGBAColor4u out { col };
        out[X] += amount;
        out[Y] += amount;
        out[Z] += amount;
        return out;
    }

    template<typename T>
    struct Rect
    {
        T x, y, w, h;

        Rect() = default;
        Rect(const T pX, const T pY, const T pW, const T pH)
            : x { pX }
            , y { pY }
            , w { pW }
            , h { pH }
        {;}

        template<class OTHER_T>
        Rect(const Rect<OTHER_T>& other)
        {
            x = (T)other.x;
            y = (T)other.y;
            w = (T)other.w;
            h = (T)other.h;
        }
    };

}//ns