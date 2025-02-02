#ifndef GFX_VECTOR2_HPP
#define GFX_VECTOR2_HPP

#include "../../External/glm/glm.hpp"
#include <string>

namespace GFX
{
    class Vector2f
    {
    public:
        static float Dot(const glm::vec2 &v1, const glm::vec2 &v2);
        static float Distance(const glm::vec2 &v1, const glm::vec2 &v2);
        static float DistanceSquared(const glm::vec2 &v1, const glm::vec2 &v2);
        static float Length(const glm::vec2 &v);
        static float LengthSquared(const glm::vec2 &v);
        static glm::vec2 Lerp(const glm::vec2 &v1, const glm::vec2 &v2, float t);
        static glm::vec2 Max(const glm::vec2 &lhs, const glm::vec2 &rhs);
        static glm::vec2 Min(const glm::vec2 &lhs, const glm::vec2 &rhs);
        static glm::vec2 Normalize(const glm::vec2 &v);
        static bool IsNan(const glm::vec2 &v);
        static std::string ToString(const glm::vec2 &v);
    };
};

#endif