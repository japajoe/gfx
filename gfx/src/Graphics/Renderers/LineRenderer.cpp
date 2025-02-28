#include "LineRenderer.hpp"
#include "../Shader.hpp"
#include "../GL.hpp"
#include "../../System/Numerics/Quaternion.hpp"
#include "../../Core/Camera.hpp"
#include "../../Core/Resources.hpp"
#include "../../External/glad/glad.h"
#include "../../External/glm/glm.hpp"

namespace GFX
{
    LineVertex::LineVertex()
    {
        this->position = Vector3(0.0f, 0.0f, 0.0f);
        this->color = Color(255, 255, 255, 255);
    }

    LineVertex::LineVertex(const Vector3 &position, const Color &color)
    {
        this->position = position;
        this->color = color;
    }

    std::vector<LineVertex> LineRenderer::lines;
    size_t LineRenderer::numLines = 0;
    size_t LineRenderer::pointIndex = 0;
    size_t LineRenderer::maxLines = 4096;
    GLuint LineRenderer::VAO = 0;
    GLuint LineRenderer::VBO = 0;
    Shader *LineRenderer::shader = nullptr;
    float LineRenderer::lineWidth = 1.0f;

    void LineRenderer::Initialize()
    {
        shader = Resources::FindShader("Line");

        size_t maxVertices = maxLines * 2;

        lines.resize(maxVertices);

        glCreateVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, lines.size() * sizeof(LineVertex), lines.data(), GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(LineVertex), (const GLvoid*)offsetof(LineVertex, position));

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, false, sizeof(LineVertex), (const GLvoid*)offsetof(LineVertex, color));

        glBindVertexArray(0);
    }

    void LineRenderer::Deinitialize()
    {
        if(VAO > 0)
        {
            glDeleteVertexArrays(1, &VAO);
            VAO = 0;
        }

        if(VBO > 0)
        {
            glDeleteBuffers(1, &VBO);
            VBO = 0;
        }
    }

    void LineRenderer::AddToDrawList(const Vector3 &p1, const Vector3 &p2, const Color &color)
    {
        if(lines.size() == 0)
        {
            Initialize();
        }

        if(numLines >= maxLines)
        {
            //Double the capacity if we run out of space
            maxLines = maxLines * 2;
            size_t maxVertices = maxLines * 2;
            std::vector<LineVertex> newLines;
            newLines.resize(maxVertices);
            for(size_t i = 0; i < lines.size(); i++)
            {
                newLines[i] = lines[i];
            }

            lines = newLines;
        }

        LineVertex lp1(p1, color);
        LineVertex lp2(p2, color);

        lines[pointIndex+0] = lp1;
        lines[pointIndex+1] = lp2;
        
        pointIndex += 2;
        numLines++;
    }

    void LineRenderer::DrawLine(const Vector3 &p1, const Vector3 &p2)
    {
        AddToDrawList(p1, p2, Color::White());
    }

    void LineRenderer::DrawLine(const Vector3 &p1, const Vector3 &p2, const Color &color)
    {
        AddToDrawList(p1, p2, color);
    }

    void LineRenderer::DrawWireArc(const Vector3 &center, const Vector3 &normal, const Vector3 &from, float angle, float radius, const Color &color)
    {
        angle = glm::radians(angle);
        const int numSegments = 36;
        float t = angle / numSegments;
        Quaternion rotation = Quaternionf::FromToRotation(Vector3f::UnitY(), normal);

        auto transformation = Matrix4f::CreateRotation(rotation.x, rotation.y, rotation.z, rotation.w);

        //Vector3 origin = center + (Vector3f::Normalize(from) * radius);
        Vector3 origin = center;

        for(int i = 0; i < numSegments; i++)
        {
            float x1 = glm::sin(t * i) * radius;
            float y1 = glm::cos(t * i) * radius;
            float x2 = glm::sin(t * (i+1)) * radius;
            float y2 = glm::cos(t * (i+1)) * radius;
            Vector3 p1 = (Vector3(x1, y1, 0) + origin);
            Vector3 p2 = (Vector3(x2, y2, 0) + origin);

            Vector4 pp1 = transformation * Vector4(p1.x, p1.y, p1.z, 1.0f);
            Vector4 pp2 = transformation * Vector4(p2.x, p2.y, p2.z, 1.0f);

            p1 = pp1;
            p2 = pp2;

            AddToDrawList(p1, p2, color);
        }
    }

    void LineRenderer::DrawSphere(const Vector3 &center, float radius)
    {
        size_t subdivisions = 16;

        //-- Generate the vertices of the sphere
        for(size_t i = 0; i < subdivisions; i++)
        {
            float theta1 = (i / subdivisions) * glm::pi<float>() * 2;
            float theta2 = ((i + 1) / subdivisions) * glm::pi<float>() * 2;

            for(size_t j = 0; j < subdivisions; j++)
            {
                float phi1 = (j / subdivisions) * glm::pi<float>();
                float phi2 = ((j + 1) / subdivisions) * glm::pi<float>();

                Vector3 v1(
                    center.x + radius * glm::sin(phi1) * glm::cos(theta1),
                    center.y + radius * glm::sin(phi1) * glm::sin(theta1),
                    center.z + radius * glm::cos(phi1)
                );

                Vector3 v2(
                    center.x + radius * glm::sin(phi1) * glm::cos(theta2),
                    center.y + radius * glm::sin(phi1) * glm::sin(theta2),
                    center.z + radius * glm::cos(phi1)
                );

                Vector3 v3(
                    center.x + radius * glm::sin(phi2) * glm::cos(theta1),
                    center.y + radius * glm::sin(phi2) * glm::sin(theta1),
                    center.z + radius * glm::cos(phi2)
                );

                Vector3 v4(
                    center.x + radius * glm::sin(phi2) * glm::cos(theta2),
                    center.y + radius * glm::sin(phi2) * glm::sin(theta2),
                    center.z + radius * glm::cos(phi2)
                );

                LineRenderer::DrawLine(v1, v2);
                LineRenderer::DrawLine(v2, v3);
                LineRenderer::DrawLine(v3, v1);

                LineRenderer::DrawLine(v2, v3);
                LineRenderer::DrawLine(v3, v4);
                LineRenderer::DrawLine(v4, v2);
            }
        }
    }

    void LineRenderer::DrawWireDisc(const Vector3 &center, const Vector3 &normal, float radius, const Color &color)
    {
        const int numSegments = 36; // Adjust this for smoother or rougher circle
        float t = (2.0 * glm::pi<float>()) / numSegments;
        Quaternion rotation = Quaternionf::FromToRotation(Vector3f::UnitY(), normal);

        auto transformation = Matrix4f::CreateRotation(rotation.x, rotation.y, rotation.z, rotation.w);

        for(int i = 0; i < numSegments; i++)
        {
            float x1 = glm::sin(t * i) * radius;
            float y1 = glm::cos(t * i) * radius;
            float x2 = glm::sin(t * (i+1)) * radius;
            float y2 = glm::cos(t * (i+1)) * radius;
            Vector3 p1 = Vector3(x1, y1, 0) + center;
            Vector3 p2 = Vector3(x2, y2, 0) + center;

            Vector4 pp1 = transformation * Vector4(p1.x, p1.y, p1.z, 1.0f);
            Vector4 pp2 = transformation * Vector4(p2.x, p2.y, p2.z, 1.0f);

            p1 = pp1;
            p2 = pp2;

            AddToDrawList(p1, p2, color);
        }
    }

    void LineRenderer::NewFrame()
    {
        if(numLines == 0)
            return;

        auto camera = Camera::GetMain();

        if(!camera)
            return;

        GLsizei numVertices = numLines * 2;

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, lines.size() * sizeof(LineVertex), lines.data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        Matrix4 model = Matrix4(1.0);
        Matrix4 view = camera->GetViewMatrix();
        Matrix4 projection = camera->GetProjectionMatrix();
        Matrix4 mvp = projection * view * model;

        GL::DepthTest(true);
        GL::BlendMode(true);

        shader->Use();

        shader->SetMat4("uModel", glm::value_ptr(model));
        shader->SetMat4("uView", glm::value_ptr(view));
        shader->SetMat4("uProjection", glm::value_ptr(projection));
        shader->SetMat4("uMVP", glm::value_ptr(mvp));

        glBindVertexArray(VAO);
        glDrawArrays(GL_LINES, 0, numVertices);
        glBindVertexArray(0);
        glUseProgram(0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        GL::DepthTest(false);
        GL::BlendMode(false);

        Clear();
    }

    void LineRenderer::Clear()
    {
        pointIndex = 0;
        numLines = 0;
    }

    void LineRenderer::SetLineWidth(float width)
    {
        lineWidth = width;
        glLineWidth(width);
    }

    float LineRenderer::GetLineWidth()
    {
        return lineWidth;
    }
}