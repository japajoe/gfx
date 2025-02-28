#ifndef GFX_VECTOR4_HPP
#define GFX_VECTOR4_HPP

#include "../../External/glm/glm.hpp"
#include <string>

namespace GFX
{
    class Vector4f
    {
    public:
        static float Dot(const glm::vec4 &v1, const glm::vec4 &v2);
        static float Distance(const glm::vec4 &v1, const glm::vec4 &v2);
        static float DistanceSquared(const glm::vec4 &v1, const glm::vec4 &v2);
        static float Length(const glm::vec4 &v);
        static float LengthSquared(const glm::vec4 &v);
        static glm::vec4 Lerp(const glm::vec4 &v1, const glm::vec4 &v2, float t);
        static glm::vec4 Min(const glm::vec4 &lhs, const glm::vec4 &rhs);
        static glm::vec4 Max(const glm::vec4 &lhs, const glm::vec4 &rhs);
        static glm::vec4 Normalize(const glm::vec4 &v);
        static bool IsNan(const glm::vec4 &v);
        static std::string ToString(const glm::vec4 &v);
    };
};

#endif