#ifndef GFX_VECTOR3_HPP
#define GFX_VECTOR3_HPP

#include "../../External/glm/glm.hpp"
#include <string>

namespace GFX
{
    class Vector3f
    {
    public:
        static glm::vec3 UnitX();
        static glm::vec3 UnitY();
        static glm::vec3 UnitZ();
        static glm::vec3 Right();
        static glm::vec3 Up();
        static glm::vec3 Forward();
        static glm::vec3 Zero();
        static glm::vec3 One();
        static glm::vec3 FromSphericalCoordinates(float radius, float phi, float theta);
        static glm::vec3 Clamp(const glm::vec3 &v, const glm::vec3 &min, const glm::vec3 &max);
        static glm::vec3 Cross(const glm::vec3 &v1, const glm::vec3 &v2);
        static float Dot(const glm::vec3 &v1, const glm::vec3 &v2);
        static float Distance(const glm::vec3 &v1, const glm::vec3 &v2);
        static float DistanceSquared(const glm::vec3 &v1, const glm::vec3 &v2);
        static float Length(const glm::vec3 &v);
        static float LengthSquared(const glm::vec3 &v);
        static glm::vec3 Lerp(const glm::vec3 &v1, const glm::vec3 &v2, float t);
        static glm::vec3 Slerp(const Vector3 &a, const Vector3 &b, float t);
        static glm::vec3 Max(const glm::vec3 &lhs, const glm::vec3 &rhs);
        static glm::vec3 Min(const glm::vec3 &lhs, const glm::vec3 &rhs);
        static glm::vec3 Normalize(const glm::vec3 &v);
        static glm::vec3 Transform(const glm::vec3 &v, const glm::quat &r);
        static glm::vec3 TransformPosition(const glm::vec3 &v, const glm::mat4 &m);
        static glm::vec3 TransformVector(const glm::vec3 &v, const glm::mat4 &m);
        static glm::vec3 Perpendicular(const glm::vec3 &v);
        static glm::vec3 ProjectOnPlane(const glm::vec3 &vector, const glm::vec3 &planeNormal);
        static float Angle(const glm::vec3 &from, const glm::vec3 &to);
        static bool IsNan(const glm::vec3 &v);
        static std::string ToString(const glm::vec3 &v);
    };
};

#endif