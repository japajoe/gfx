#include "Matrix4.hpp"

namespace GFX
{
    glm::mat4 Matrix4f::GetIdentity()
    {
        return glm::mat4();
    }

    glm::mat4 Matrix4f::Perspective(float fovDegrees, float aspectRatio, float near, float far)
    {
        return glm::perspective(glm::radians(fovDegrees), aspectRatio, near, far);
    }

    glm::mat4 Matrix4f::Orthographic(float left, float right, float bottom, float top, float near, float far)
    {
        if(near > 0 && far > 0)
            return glm::ortho(left, right, bottom, top, near, far);
        else
            return glm::ortho(left, right, bottom, top);
    }

    glm::mat4 Matrix4f::CreateTranslation(float x, float y, float z)
    {
        return glm::translate(glm::vec3(x, y, z));
    }

    glm::mat4 Matrix4f::CreateRotation(float x, float y, float z)
    {
        return glm::eulerAngleXYZ(x, y, z);
    }

    glm::mat4 Matrix4f::CreateRotation(float x, float y, float z, float w)
    {
        return glm::toMat4(glm::quat(w, x, y, z));
    }

    glm::mat4 Matrix4f::CreateRotationX(float angle)
    {
        return glm::rotate(glm::mat4(1.0f), angle, glm::vec3(1.0f, 0.0f, 0.0f));
    }

    glm::mat4 Matrix4f::CreateRotationY(float angle)
    {
        return glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f));
    }

    glm::mat4 Matrix4f::CreateRotationZ(float angle)
    {
        return glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 0.0f, 1.0f));
    }

    glm::mat4 Matrix4f::CreateScale(float x, float y, float z)
    {
        return glm::scale(glm::vec3(x, y, z));
    }

    glm::quat Matrix4f::ExtractRotation(const glm::mat4 &v)
    {
        glm::vec3 pos = v[3];
        glm::vec3 scale;
        for(int i = 0; i < 3; i++)
            scale[i] = glm::length(glm::vec3(v[i]));
        const glm::mat3 rotMtx(
            glm::vec3(v[0]) / scale[0],
            glm::vec3(v[1]) / scale[1],
            glm::vec3(v[2]) / scale[2]);
        glm::quat rot = glm::quat_cast(rotMtx);
        return rot;

        // glm::vec3 row0 = v[0];
        // glm::vec3 row1 = v[1];
        // glm::vec3 row2 = v[2];

        // const bool rowNormalize = true;
        // if (rowNormalize)
        // {
        //     row0 = glm::normalize(row0);
        //     row1 = glm::normalize(row1);
        //     row2 = glm::normalize(row2);
        // }

        // // code below adapted from Blender
        // glm::quat q = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        // float trace = 0.25f * (row0.x + row1.y + row2.z + 1.0f);

        // if (trace > 0)
        // {
        //     float sq = glm::sqrt(trace);

        //     q.w = sq;
        //     sq = 1.0f / (4.0f * sq);
        //     q.x = ((row1.z - row2.y) * sq);
        //     q.y = ((row2.x - row0.z) * sq);
        //     q.z = ((row0.y - row1.x) * sq);
        // }
        // else if (row0.x > row1.y && row0.x > row2.z)
        // {
        //     float sq = 2.0f * glm::sqrt(1.0f + row0.x - row1.y - row2.z);

        //     q.x = (0.25f * sq);
        //     sq = 1.0f / sq;
        //     q.w = ((row2.y - row1.z) * sq);
        //     q.y = ((row1.x + row0.y) * sq);
        //     q.z = ((row2.x + row0.z) * sq);
        // }
        // else if (row1.y > row2.z)
        // {
        //     float sq = 2.0f * glm::sqrt(1.0f + row1.y - row0.x - row2.z);

        //     q.y = (0.25f * sq);
        //     sq = 1.0f / sq;
        //     q.w = ((row2.x - row0.z) * sq);
        //     q.x = ((row1.x + row0.y) * sq);
        //     q.z = ((row2.y + row1.z) * sq);
        // }
        // else
        // {
        //     float sq = 2.0f * glm::sqrt(1.0f + row2.z - row0.x - row1.y);

        //     q.z = (0.25f * sq);
        //     sq = 1.0f / sq;
        //     q.w = ((row1.x - row0.y) * sq);
        //     q.x = ((row2.x + row0.z) * sq);
        //     q.z = ((row2.y + row1.z) * sq);
        // }

        // q = glm::normalize(q);
        // return q;
    }

    glm::vec3 Matrix4f::ExtractScale(const glm::mat4 &v)
    {
        glm::vec3 scale;
        for(int i = 0; i < 3; i++)
            scale[i] = glm::length(glm::vec3(v[i]));
        return scale;

        // glm::vec3 scale;
        // scale.x = glm::length(glm::vec3(v[0][0], v[0][1], v[0][2]));
        // scale.y = glm::length(glm::vec3(v[1][0], v[1][1], v[1][2]));
        // scale.z = glm::length(glm::vec3(v[2][0], v[2][1], v[2][2]));
        // return scale;
    }

    glm::vec3 Matrix4f::ExtractTranslation(const glm::mat4 &v)
    {
        glm::vec3 pos = v[3];
        return pos;

        // glm::vec3 translation;
        // translation.x = v[3][0];
        // translation.y = v[3][1];
        // translation.z = v[3][2];
        // return translation;
    }

    glm::mat4 Matrix4f::Identity()
    {
        return glm::mat4(1.0f);
    }

    glm::mat4 Matrix4f::Invert(const glm::mat4 &v)
    {
        return glm::inverse(v);
    }

    glm::mat4 Matrix4f::LookAt(const glm::vec3 &eye, const glm::vec3 &target, const glm::vec3 &up)
    {
        return glm::lookAt(eye, target, up);
    }
};