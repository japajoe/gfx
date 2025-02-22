#ifndef GFX_QUATERNION_HPP
#define GFX_QUATERNION_HPP

#include "../../External/glm/glm.hpp"
#include <string>

namespace GFX
{
    class Quaternionf
    {
    public:
        static glm::quat Conjugate(const glm::quat &v);
        static glm::quat Euler(float x, float y, float z);
        static glm::quat Euler(const glm::vec3 eulerAngles);
        static glm::quat FromAxisAngle(const glm::vec3 &axis, float angle);
        static glm::quat FromMatrix(const glm::mat3 &v);
        static glm::quat FromMatrix(const glm::mat4 &v);
        static glm::quat FromToRotation(const glm::vec3 &from, const glm::vec3 & to);
        static glm::quat Identity();
        static glm::quat Invert(const glm::quat &v);
        static float Length(const glm::quat &v);
        static float LengthSquared(const glm::quat &v);
        static glm::quat LookRotation(glm::vec3 forward, glm::vec3 upwards = glm::vec3(0.0f, 1.0f, 0.0f));
        static glm::quat Normalize(const glm::quat &v);
        static glm::quat Slerp(const glm::quat &v1, const glm::quat &v2, float t);
        static glm::vec3 ToEulerAngles(const glm::quat &v);
        static bool IsNan(const glm::quat &v);
        static std::string ToString(const glm::quat &v);
    };
};

#endif