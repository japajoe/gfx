#ifndef GFX_RAY_HPP
#define GFX_RAY_HPP

#include "../System/Numerics/Vector2.hpp"
#include "../System/Numerics/Vector3.hpp"
#include "../System/Numerics/Vector4.hpp"
#include "Rectangle.hpp"

namespace GFX
{
    struct Ray
    {
        Vector3 origin;
        Vector3 direction;
        float length;
        Ray();
        Ray(const Vector3 &origin, const Vector3 &direction, float length);
        static Ray FromMousePosition(const Vector2 &mousePosition, const Rectangle &viewportRect);
    };
}

#endif