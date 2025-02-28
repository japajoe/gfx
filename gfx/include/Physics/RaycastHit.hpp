#ifndef GFX_RAYCASTHIT_HPP
#define GFX_RAYCASTHIT_HPP

#include "../System/Numerics/Vector2.hpp"
#include "../System/Numerics/Vector3.hpp"
#include "../System/Numerics/Vector4.hpp"
#include "../Graphics/Rectangle.hpp"
#include "../Core/Transform.hpp"
#include <cstdint>

namespace GFX
{
    struct RaycastHit
    {
        float distance;
        Vector3 point;
        Vector3 normal;
        Transform *transform;
        uint32_t triangleIndex1;
        uint32_t triangleIndex2;
        uint32_t triangleIndex3;
        void *userData;
    };

    struct Ray
    {
        Vector3 origin;
        Vector3 direction;
        float length;
        Ray();
        Ray(const Vector3 &origin, const Vector3 &direction, float length);
        //static Ray FromMousePosition(const Vector2 &mousePosition, const Rectangle &viewportRect);
        static Ray FromMousePosition();
    };
}

#endif