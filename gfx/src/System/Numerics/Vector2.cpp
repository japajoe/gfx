#include "Vector2.hpp"

namespace GFX
{
    float Vector2f::Dot(const glm::vec2 &v1, const glm::vec2 &v2)
    {
        return glm::dot(v1, v2);
    }

    float Vector2f::Distance(const glm::vec2 &v1, const glm::vec2 &v2)
    {
        return glm::distance(v1, v2);
    }

    float Vector2f::DistanceSquared(const glm::vec2 &v1, const glm::vec2 &v2)
    {
        return glm::distance2(v1, v2);
    }

    float Vector2f::Length(const glm::vec2 &v)
    {
        return glm::length(v);
    }

    float Vector2f::LengthSquared(const glm::vec2 &v)
    {
        return glm::length2(v);
    }

    glm::vec2 Vector2f::Lerp(const glm::vec2 &v1, const glm::vec2 &v2, float t)
    {
        return glm::lerp(v1, v2, t);
    }

    glm::vec2 Vector2f::Max(const glm::vec2 &lhs, const glm::vec2 &rhs)
    {
        return glm::vec2(glm::max(lhs.x, rhs.x), glm::max(lhs.y, rhs.y));
    }

    glm::vec2 Vector2f::Min(const glm::vec2 &lhs, const glm::vec2 &rhs)
    {
        return glm::vec2(glm::min(lhs.x, rhs.x), glm::min(lhs.y, rhs.y));
    }

    glm::vec2 Vector2f::Normalize(const glm::vec2 &v)
    {
        return glm::normalize(v);
    }

    bool Vector2f::IsNan(const glm::vec2 &v)
    {
        if(std::isnan(v.x) || std::isnan(v.y))
            return true;
        return false;
    }

    std::string Vector2f::ToString(const glm::vec2 &v)
    {
        std::string s = std::to_string(v.x) + "," + 
                        std::to_string(v.y);
        return s;
    }
};