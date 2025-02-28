#include "Vector4.hpp"

namespace GFX
{
    float Vector4f::Dot(const glm::vec4 &v1, const glm::vec4 &v2)
    {
        return glm::dot(v1, v2);
    }

    float Vector4f::Distance(const glm::vec4 &v1, const glm::vec4 &v2)
    {
        return glm::distance(v1, v2);
    }

    float Vector4f::DistanceSquared(const glm::vec4 &v1, const glm::vec4 &v2)
    {
        return glm::distance2(v1, v2);
    }

    float Vector4f::Length(const glm::vec4 &v)
    {
        return glm::length(v);
    }

    float Vector4f::LengthSquared(const glm::vec4 &v)
    {
        return glm::length2(v);
    }

    glm::vec4 Vector4f::Lerp(const glm::vec4 &v1, const glm::vec4 &v2, float t)
    {
        return glm::lerp(v1, v2, t);
    }

    glm::vec4 Vector4f::Max(const glm::vec4 &lhs, const glm::vec4 &rhs)
    {
        return glm::vec4(glm::max(lhs.x, rhs.x), glm::max(lhs.y, rhs.y), glm::max(lhs.z, rhs.z), glm::max(lhs.w, rhs.w));
    }

    glm::vec4 Vector4f::Min(const glm::vec4 &lhs, const glm::vec4 &rhs)
    {
        return glm::vec4(glm::min(lhs.x, rhs.x), glm::min(lhs.y, rhs.y), glm::min(lhs.z, rhs.z), glm::min(lhs.w, rhs.w));
    }

    glm::vec4 Vector4f::Normalize(const glm::vec4 &v)
    {
        return glm::normalize(v);
    }

    bool Vector4f::IsNan(const glm::vec4 &v)
    {
        if(std::isnan(v.x) || std::isnan(v.y) || std::isnan(v.z) || std::isnan(v.w))
            return true;
        return false;
    }

    std::string Vector4f::ToString(const glm::vec4 &v)
    {
        std::string s = std::to_string(v.x) + "," + 
                        std::to_string(v.y) + "," + 
                        std::to_string(v.z) + "," + 
                        std::to_string(v.w);
        return s;
    }
};