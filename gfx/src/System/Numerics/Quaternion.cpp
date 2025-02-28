#include "Quaternion.hpp"

namespace GFX
{
    glm::quat Quaternionf::Conjugate(const glm::quat &v)
    {
        return glm::conjugate(v);
    }

    glm::quat Quaternionf::Euler(float x, float y, float z)
    {
        return glm::quat(glm::vec3(x, y, z));
    }

    glm::quat Quaternionf::Euler(const glm::vec3 eulerAngles)
    {
        return glm::quat(eulerAngles);
    }

    glm::quat Quaternionf::FromAxisAngle(const glm::vec3 &axis, float angle)
    {
        return glm::angleAxis(angle, axis);
    }

    glm::quat Quaternionf::FromMatrix(const glm::mat3 &v)
    {
        return glm::quat_cast(v);
    }

    glm::quat Quaternionf::FromMatrix(const glm::mat4 &v)
    {
        return glm::quat_cast(v);
    }

    glm::quat Quaternionf::FromToRotation(const glm::vec3 &from, const glm::vec3 & to)
    {
        glm::vec3 fromNormalized = glm::normalize(from);
        glm::vec3 toNormalized = glm::normalize(to);
        return glm::rotation(fromNormalized, toNormalized);
    }

    glm::quat Quaternionf::Identity()
    {
        return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    }

    glm::quat Quaternionf::Invert(const glm::quat &v)
    {
        return glm::inverse(v);
    }

    float Quaternionf::Length(const glm::quat &v)
    {
        return glm::length(v);
    }

    float Quaternionf::LengthSquared(const glm::quat &v)
    {
        return glm::length2(v);
    }

    glm::quat Quaternionf::LookRotation(glm::vec3 forward, glm::vec3 upwards)
    {
        if (glm::length(forward) < 0.0001f) 
        {
            return glm::quat(); // Return identity if forward is zero
        }

        // Normalize forward and upwards vectors
        forward = glm::normalize(forward);
        upwards = glm::normalize(upwards);

        // Check if forward and upwards are colinear or if upwards is zero
        if (glm::length(glm::cross(forward, upwards)) < 0.0001f || glm::length(upwards) < 0.0001f) 
        {
            // Use Quaternion::FromToRotation with fromDirection as positive Z-axis (0, 0, 1) and toDirection as normalized forward direction
            glm::vec3 fromDir(0.0f, 0.0f, 1.0f);
            glm::vec3 toDir = forward;
            return glm::rotation(fromDir, toDir);
        }

        // Calculate the Z axis
        glm::vec3 zAxis = forward;

        // Calculate the X axis
        glm::vec3 xAxis = glm::normalize(glm::cross(upwards, zAxis));

        // Calculate the Y axis
        glm::vec3 yAxis = glm::cross(zAxis, xAxis);

        // Create rotation matrix
        glm::mat3 rotationMatrix(xAxis, yAxis, zAxis);

        // Convert rotation matrix to quaternion
        return glm::quat(rotationMatrix);
    }

    glm::quat Quaternionf::Normalize(const glm::quat &v)
    {
        return glm::normalize(v);
    }

    glm::quat Quaternionf::Slerp(const glm::quat &v1, const glm::quat &v2, float t)
    {
        return glm::slerp(v1, v2, t);
    }

    glm::vec3 Quaternionf::ToEulerAngles(const glm::quat &v)
    {
        return glm::eulerAngles(v);
    }

    bool Quaternionf::IsNan(const glm::quat &v)
    {
        if(std::isnan(v.x) || std::isnan(v.y) || std::isnan(v.z) || std::isnan(v.w))
            return true;
        return false;
    }

    std::string Quaternionf::ToString(const glm::quat &v)
    {
        std::string s = std::to_string(v.x) + "," + 
                        std::to_string(v.y) + "," + 
                        std::to_string(v.z) + "," + 
                        std::to_string(v.w);
        return s;
    }
};