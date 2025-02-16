#include "Mesh.hpp"
#include "../External/glad/glad.h"
#include "../External/glm/glm.hpp"
#include <functional>

namespace GFX
{
    Mesh::Mesh()
    {
        sizeOfVertices = 0;
        sizeOfIndices = 0;
    }

    Mesh::Mesh(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices, bool calculateNormals)
    {
        this->vertices = vertices;
        this->indices = indices;

        sizeOfVertices = this->vertices.size();
        sizeOfIndices = this->indices.size();

        if(calculateNormals)
            RecalculateNormals();
    }

    Mesh::Mesh(const Mesh &other)
    {
        vertices = other.vertices;
        indices = other.indices;
        sizeOfVertices = other.sizeOfVertices;
        sizeOfIndices = other.sizeOfIndices;
        VAO = other.VAO;
        VBO = other.VBO;
        EBO = other.EBO;
        bounds = other.bounds;
        name = other.name;
    }

    Mesh::Mesh(Mesh &&other) noexcept
    {
        vertices = std::move(other.vertices);
        indices = std::move(other.indices);
        sizeOfVertices = std::exchange(other.sizeOfVertices, 0);
        sizeOfIndices = std::exchange(other.sizeOfIndices, 0);
        VAO = std::move(other.VAO);
        VBO = std::move(other.VBO);
        EBO = std::move(other.EBO);
        bounds = std::move(other.bounds);
        name = std::move(other.name);
    }

    Mesh& Mesh::operator=(const Mesh &other)
    {
        if(this != &other)
        {
            vertices = other.vertices;
            indices = other.indices;
            sizeOfVertices = other.sizeOfVertices;
            sizeOfIndices = other.sizeOfIndices;
            VAO = other.VAO;
            VBO = other.VBO;
            EBO = other.EBO;
            bounds = other.bounds;
            name = other.name;
        }
        return *this;
    }

    Mesh& Mesh::operator=(Mesh &&other) noexcept
    {
        if(this != &other)
        {
            vertices = std::move(other.vertices);
            indices = std::move(other.indices);
            sizeOfVertices = std::exchange(other.sizeOfVertices, 0);
            sizeOfIndices = std::exchange(other.sizeOfIndices, 0);
            VAO = std::move(other.VAO);
            VBO = std::move(other.VBO);
            EBO = std::move(other.EBO);
            bounds = std::move(other.bounds);
            name = std::move(other.name);
        }
        return *this;
    }

    std::vector<Vertex> &Mesh::GetVertices()
    {
        return vertices;
    }

    std::vector<uint32_t> &Mesh::GetIndices()
    {
        return indices;
    }

    VertexArrayObject *Mesh::GetVAO()
    {
        return &VAO;
    }

    VertexBufferObject *Mesh::GetVBO()
    {
        return &VBO;
    }

    ElementBufferObject *Mesh::GetEBO()
    {
        return &EBO;
    }

    BoundingBox Mesh::GetBounds() const
    {
        return bounds;
    }

    void Mesh::Generate()
    {
        auto &vertices = GetVertices();
        auto &indices = GetIndices();

        if(VAO.GetId() == 0 && VBO.GetId() == 0 && EBO.GetId() == 0)
        {
            VAO.Generate();
            VAO.Bind();

            VBO.Generate();
            VBO.Bind();
            VBO.BufferData(vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

            VAO.EnableVertexAttribArray(0);
            VAO.VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, position));

            VAO.EnableVertexAttribArray(1);
            VAO.VertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, normal));

            VAO.EnableVertexAttribArray(2);
            VAO.VertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, uv));

            if(indices.size() > 0)
            {
                EBO.Generate();
                EBO.Bind();                
                EBO.BufferData(indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
            }

            VAO.Unbind();
        }
        else
        {
            VBO.Bind();

            VBO.BufferData(vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

            // if(sizeOfVertices == vertices.size())
            //     VBO.BufferSubData(0, vertices.size() * sizeof(Vertex), vertices.data());
            // else
            //     VBO.BufferData(vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

            sizeOfVertices = vertices.size();

            if(indices.size() > 0)
            {
                EBO.Bind();

                // if(sizeOfIndices == indices.size())
                //     EBO.BufferSubData(0, indices.size() * sizeof(GLuint), indices.data());
                // else
                //     EBO.BufferData(indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

                EBO.BufferData(indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

                sizeOfIndices = indices.size();

                EBO.Unbind();
            }

            VBO.Unbind();
        }

        bounds.Clear();

        for(size_t i = 0; i < vertices.size(); i++)
        {
            bounds.Grow(vertices[i].position);
        }
    }

    void Mesh::Delete()
    {
        EBO.Delete();
        VBO.Delete();
        VAO.Delete();
    }

    size_t Mesh::GetVerticesCount() const
    {
        return vertices.size();
    }

    size_t Mesh::GetIndicesCount() const
    {
        return indices.size();
    }

    void Mesh::SetName(const std::string &name)
    {
        this->name = name;
    }

    std::string Mesh::GetName() const
    {
        return name;
    }

    void Mesh::RecalculateNormals()
    {
        size_t triangleCount = indices.size() / 3;

        for (size_t i = 0; i < triangleCount; i++)
        {
            int normalTriangleIndex = i * 3;
            int vertexIndexA = (int)indices[normalTriangleIndex];
            int vertexIndexB = (int)indices[normalTriangleIndex + 1];
            int vertexIndexC = (int)indices[normalTriangleIndex + 2];
            
            Vector3 triangleNormal = SurfaceNormalFromIndices(vertexIndexA, vertexIndexB, vertexIndexC);

            Vertex &v1 = vertices[vertexIndexA];
            Vertex &v2 = vertices[vertexIndexB];
            Vertex &v3 = vertices[vertexIndexC];

            v1.normal += triangleNormal;
            v2.normal += triangleNormal;
            v3.normal += triangleNormal;

            v1.normal = Vector3f::Normalize(v1.normal);
            v2.normal = Vector3f::Normalize(v2.normal);
            v3.normal = Vector3f::Normalize(v3.normal);
        }
    }

    Vector3 Mesh::SurfaceNormalFromIndices(int indexA, int indexB, int indexC)
    {
        Vector3 pA = vertices[indexA].position;
        Vector3 pB = vertices[indexB].position;
        Vector3 pC = vertices[indexC].position;

        Vector3 sideAB = pB - pA;
        Vector3 sideAC = pC - pA;
        return Vector3f::Normalize(Vector3f::Cross(sideAB, sideAC));
    }

    static Vector3 PointOnSpheroid(float radius, float height, float horizontalAngle, float verticalAngle)
    {
        float horizontalRadians = glm::radians(horizontalAngle);
        float verticalRadians = glm::radians(verticalAngle);
        float cosVertical = glm::cos(verticalRadians);

        return Vector3(
            radius * glm::sin(horizontalRadians) * cosVertical,
            height * glm::sin(verticalRadians),
            radius * glm::cos(horizontalRadians) * cosVertical);
    }

    static Vector3 PointOnSphere(float radius, float horizontalAngle, float verticalAngle)
    {
        return PointOnSpheroid(radius, radius, horizontalAngle, verticalAngle);
    }

    void MeshGenerator::SetScale(std::vector<Vertex> &vertices, const Vector3 &scale)
    {
        for(size_t i = 0; i < vertices.size(); i++)
            vertices[i].position *= scale;
    }

    Mesh MeshGenerator::Combine(const std::vector<Mesh*> &meshes)
    {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        uint32_t indiceOffset = 0;

        for(size_t i = 0; i < meshes.size(); i++)
        {
            auto &mVertices = meshes[i]->GetVertices();

            for(size_t j = 0; j < mVertices.size(); j++)
            {
                vertices.push_back(mVertices[j]);
            }

            auto &mIndices = meshes[i]->GetIndices();

            for(size_t j = 0; j < mIndices.size(); j++)
            {
                indices.push_back(mIndices[j] + indiceOffset);
            }
            
            indiceOffset += (uint32_t)meshes[i]->GetVerticesCount();
        }

        Mesh mesh(vertices, indices, true);
        mesh.Generate();
        return mesh;
    }

    Mesh MeshGenerator::CreateCapsule(const Vector3 &scale)
    {
        float height = 2.0f;
        float radius = 0.5f;   
        int segments = 32;
        int rings = 8;
        float cylinderHeight = height - radius * 2;
        int vertexCount = 2 * rings * segments + 2;
        int triangleCount = 4 * rings * segments;
        float horizontalAngle = 360.0f / segments;
        float verticalAngle = 90.0f / rings;

        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;

        vertices.resize(vertexCount);
        indices.resize(3 * triangleCount);

        int vi = 2;
        int ti = 0;
        int topCapIndex = 0;
        int bottomCapIndex = 1;

        vertices[topCapIndex].position = Vector3(0, cylinderHeight / 2 + radius, 0);
        vertices[topCapIndex].normal = Vector3(0, 1, 0);
        vertices[bottomCapIndex].position = Vector3(0, -cylinderHeight / 2 - radius, 0);
        vertices[bottomCapIndex].normal = Vector3(0, -1, 0);

        for (int s = 0; s < segments; s++)
        {
            for (int r = 1; r <= rings; r++)
            {
                // Top cap vertex
                Vector3 normal = PointOnSphere(1, s*horizontalAngle, 90 - r * verticalAngle);
                Vector3 vertex = Vector3(radius*normal.x, radius * normal.y + cylinderHeight / 2, radius * normal.z);
                vertices[vi].position = vertex;
                vertices[vi].normal = normal;
                vi++;

                // Bottom cap vertex
                vertices[vi].position = Vector3(vertex.x, -vertex.y, vertex.z);
                vertices[vi].normal = Vector3(normal.x, -normal.y, normal.z);
                vi++;

                int top_s1r1 = vi - 2;
                int top_s1r0 = vi - 4;
                int bot_s1r1 = vi - 1;
                int bot_s1r0 = vi - 3;
                int top_s0r1 = top_s1r1 - 2 * rings;
                int top_s0r0 = top_s1r0 - 2 * rings;
                int bot_s0r1 = bot_s1r1 - 2 * rings;
                int bot_s0r0 = bot_s1r0 - 2 * rings;

                if (s == 0)
                {
                    top_s0r1 += vertexCount - 2;
                    top_s0r0 += vertexCount - 2;
                    bot_s0r1 += vertexCount - 2;
                    bot_s0r0 += vertexCount - 2;
                }

                // Create cap triangles
                if (r == 1)
                {
                    indices[3 * ti + 0] = topCapIndex;
                    indices[3 * ti + 1] = top_s0r1;
                    indices[3 * ti + 2] = top_s1r1;
                    ti++;

                    indices[3 * ti + 0] = bottomCapIndex;
                    indices[3 * ti + 1] = bot_s1r1;
                    indices[3 * ti + 2] = bot_s0r1;
                    ti++;
                }
                else
                {
                    indices[3 * ti + 0] = top_s1r0;
                    indices[3 * ti + 1] = top_s0r0;
                    indices[3 * ti + 2] = top_s1r1;
                    ti++;

                    indices[3 * ti + 0] = top_s0r0;
                    indices[3 * ti + 1] = top_s0r1;
                    indices[3 * ti + 2] = top_s1r1;
                    ti++;

                    indices[3 * ti + 0] = bot_s0r1;
                    indices[3 * ti + 1] = bot_s0r0;
                    indices[3 * ti + 2] = bot_s1r1;
                    ti++;

                    indices[3 * ti + 0] = bot_s0r0;
                    indices[3 * ti + 1] = bot_s1r0;
                    indices[3 * ti + 2] = bot_s1r1;
                    ti++;
                }
            }

            // Create side triangles
            int top_s1 = vi - 2;
            int top_s0 = top_s1 - 2 * rings;
            int bot_s1 = vi - 1;
            int bot_s0 = bot_s1 - 2 * rings;
            
            if (s == 0)
            {
                top_s0 += vertexCount - 2;
                bot_s0 += vertexCount - 2;
            }

            indices[3 * ti + 0] = top_s0;
            indices[3 * ti + 1] = bot_s1;
            indices[3 * ti + 2] = top_s1;
            ti++;

            indices[3 * ti + 0] = bot_s0;
            indices[3 * ti + 1] = bot_s1;
            indices[3 * ti + 2] = top_s0;
            ti++;
        }

        SetScale(vertices, scale);
        Mesh mesh(vertices, indices, true);
        mesh.Generate();
        return mesh;
    }

    Mesh MeshGenerator::CreateCube(const Vector3 &scale)
    {
        std::vector<Vertex> vertices;
        vertices.resize(24);

        vertices[0].position = Vector3(0.5f, -0.5f, 0.5f);
        vertices[1].position = Vector3(-0.5f, -0.5f, 0.5f);
        vertices[2].position = Vector3(0.5f, 0.5f, 0.5f);
        vertices[3].position = Vector3(-0.5f, 0.5f, 0.5f);

        vertices[4].position = Vector3(0.5f, 0.5f, -0.5f);
        vertices[5].position = Vector3(-0.5f, 0.5f, -0.5f);
        vertices[6].position = Vector3(0.5f, -0.5f, -0.5f);
        vertices[7].position = Vector3(-0.5f, -0.5f, -0.5f);

        vertices[8].position = Vector3(0.5f, 0.5f, 0.5f);
        vertices[9].position = Vector3(-0.5f, 0.5f, 0.5f);
        vertices[10].position = Vector3(0.5f, 0.5f, -0.5f);
        vertices[11].position = Vector3(-0.5f, 0.5f, -0.5f);

        vertices[12].position = Vector3(0.5f, -0.5f, -0.5f);
        vertices[13].position = Vector3(0.5f, -0.5f, 0.5f);
        vertices[14].position = Vector3(-0.5f, -0.5f, 0.5f);
        vertices[15].position = Vector3(-0.5f, -0.5f, -0.5f);

        vertices[16].position = Vector3(-0.5f, -0.5f, 0.5f);
        vertices[17].position = Vector3(-0.5f, 0.5f, 0.5f);
        vertices[18].position = Vector3(-0.5f, 0.5f, -0.5f);
        vertices[19].position = Vector3(-0.5f, -0.5f, -0.5f);

        vertices[20].position = Vector3(0.5f, -0.5f, -0.5f);
        vertices[21].position = Vector3(0.5f, 0.5f, -0.5f);
        vertices[22].position = Vector3(0.5f, 0.5f, 0.5f);
        vertices[23].position = Vector3(0.5f, -0.5f, 0.5f);

        // UVS
        vertices[0].uv = Vector2(0.0f, 0.0f);
        vertices[1].uv = Vector2(1.0f, 0.0f);
        vertices[2].uv = Vector2(0.0f, 1.0f);
        vertices[3].uv = Vector2(1.0f, 1.0f);

        vertices[4].uv = Vector2(0.0f, 1.0f);
        vertices[5].uv = Vector2(1.0f, 1.0f);
        vertices[6].uv = Vector2(0.0f, 1.0f);
        vertices[7].uv = Vector2(1.0f, 1.0f);
        
        vertices[8].uv = Vector2(0.0f, 0.0f);
        vertices[9].uv = Vector2(1.0f, 0.0f);
        vertices[10].uv = Vector2(0.0f, 0.0f);
        vertices[11].uv = Vector2(1.0f, 0.0f);
        
        vertices[12].uv = Vector2(0.0f, 0.0f);
        vertices[13].uv = Vector2(0.0f, 1.0f);
        vertices[14].uv = Vector2(1.0f, 1.0f);
        vertices[15].uv = Vector2(1.0f, 0.0f);
        
        vertices[16].uv = Vector2(0.0f, 0.0f);
        vertices[17].uv = Vector2(0.0f, 1.0f);
        vertices[18].uv = Vector2(1.0f, 1.0f);
        vertices[19].uv = Vector2(1.0f, 0.0f);
        
        vertices[20].uv = Vector2(0.0f, 0.0f);
        vertices[21].uv = Vector2(0.0f, 1.0f);
        vertices[22].uv = Vector2(1.0f, 1.0f);
        vertices[23].uv = Vector2(1.0f, 0.0f);

        std::vector<GLuint> indices =
        {
            0, 2, 3,
            0, 3, 1,

            8, 4, 5,
            8, 5, 9,

            10, 6, 7,
            10, 7, 11,

            12, 13, 14,
            12, 14, 15,

            16, 17, 18,
            16, 18, 19,

            20, 21, 22,
            20, 22, 23
        };

        SetScale(vertices, scale);
        Mesh mesh(vertices, indices, true);
        mesh.Generate();
        return mesh;
    }

Mesh MeshGenerator::CreateCylinder(const Vector3 &scale)
{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    const float height = 1.0f;
    const float radius = 0.5f;
    const uint32_t slices = 12;
    float halfHeight = height / 2.0F;

    // Generate the side vertices.
    for (uint32_t slice = 0; slice <= slices; slice++) 
    {
        float angle = slice * glm::tau<float>() / slices;
        float x = glm::cos(angle) * (radius / 2.0F);
        float z = glm::sin(angle) * (radius / 2.0F);
        float u = (float) slice / slices;

        // Bottom vertex.
        vertices.push_back(Vertex(  Vector3(x, -halfHeight, z),
                                    Vector3f::Normalize(Vector3(x, 0.0F, z)),
                                    Vector2(u, 1.0F)));

        // Top vertex.
        vertices.push_back(Vertex(  Vector3(x, halfHeight, z),
                                    Vector3f::Normalize(Vector3(x, 0.0F, z)),
                                    Vector2(u, 0.0F)));
    }

    // Generate the side indices with flipped winding order.
    for (uint32_t slice = 0; slice < slices; slice++) 
    {
        uint32_t baseIndex = slice * 2;
        uint32_t nextIndex = (slice + 1) * 2;

        // Flip the winding order: swap the order of the indices
        indices.push_back(baseIndex);
        indices.push_back(baseIndex + 1);
        indices.push_back(nextIndex);

        indices.push_back(nextIndex);
        indices.push_back(baseIndex + 1);
        indices.push_back(nextIndex + 1);
    }

    // Generate the bottom cap with flipped winding order.
    size_t bottomCenterIndex = vertices.size();

    vertices.push_back(Vertex(  Vector3(0, -halfHeight, 0),
                                -Vector3f::UnitY(),
                                Vector2(0.5F, 0.5F)));

    for (uint32_t slice = 0; slice < slices; slice++) 
    {
        uint32_t baseIndex = slice * 2;

        // Flip the winding order
        indices.push_back(bottomCenterIndex);
        indices.push_back(baseIndex);
        indices.push_back(baseIndex + 2);
    }

    // Generate the top cap with flipped winding order.
    int topCenterIndex = vertices.size();

    vertices.push_back(Vertex(  Vector3(0, halfHeight, 0),
                                Vector3f::UnitY(),
                                Vector2(0.5F, 0.5F)));

    for (uint32_t slice = 0; slice < slices; slice++) 
    {
        uint32_t baseIndex = slice * 2 + 1;

        // Flip the winding order
        indices.push_back(topCenterIndex);
        indices.push_back(baseIndex + 2);
        indices.push_back(baseIndex);
    }

    SetScale(vertices, scale);
    Mesh mesh(vertices, indices, true);
    mesh.Generate();
    return mesh;
}


    Mesh MeshGenerator::CreatePlane(const Vector3 &scale)
    {
        std::vector<Vertex> vertices;
        vertices.resize(4);

        Vertex bottomLeft;
        Vertex bottomRight;
        Vertex topleft;
        Vertex topRight;

        topleft.position = Vector3(-0.5f, 0.0f, -0.5f);
        bottomLeft.position = Vector3(-0.5f, 0.0f, 0.5f);
        bottomRight.position = Vector3(0.5f, 0.0f, 0.5f);
        topRight.position = Vector3(0.5f, 0.0f, -0.5f);

        //topleft.uv = Vector2(0.0f, 1.0f);
        //bottomLeft.uv = Vector2(0.0f, 0.0f);
        //bottomRight.uv = Vector2(1.0f, 0.0f);
        //topRight.uv = Vector2(1.0f, 1.0f);

        topleft.uv = Vector2(0.0f, 0.0f);
        bottomLeft.uv = Vector2(0.0f, 1.0f);
        bottomRight.uv = Vector2(1.0f, 1.0f);
        topRight.uv = Vector2(1.0f, 0.0f);

        vertices[0] = topleft;
        vertices[1] = bottomLeft;
        vertices[2] = topRight;
        vertices[3] = bottomRight;

        std::vector<GLuint> indices =
        {
            0, 1, 2,
            2, 1, 3,
        };

        SetScale(vertices, scale);
        Mesh mesh(vertices, indices, true);
        mesh.Generate();
        return mesh;
    }

    Mesh MeshGenerator::CreateQuad(const Vector3 &scale)
    {
        std::vector<Vertex> vertices;
        vertices.resize(4);

        Vertex bottomLeft;
        Vertex bottomRight;
        Vertex topleft;
        Vertex topRight;

        topleft.position     = Vector3(0.0f, 1.0f, 0.0f);
        bottomLeft.position  = Vector3(0.0f, 0.0f, 0.0f);
        bottomRight.position = Vector3(1.0f, 0.0f, 0.0f);
        topRight.position    = Vector3(1.0f, 1.0f, 0.0f);

        topleft.uv           = Vector2(0.0f, 1.0f);
        bottomLeft.uv        = Vector2(0.0f, 0.0f);
        bottomRight.uv       = Vector2(1.0f, 0.0f);
        topRight.uv          = Vector2(1.0f, 1.0f);

        topleft.normal       = Vector3(0, 1, 0);
        bottomLeft.normal    = Vector3(0, 1, 0);
        bottomRight.normal   = Vector3(0, 1, 0);
        topRight.normal      = Vector3(0, 1, 0);

        vertices[0] = topleft;
        vertices[1] = bottomLeft;
        vertices[2] = topRight;
        vertices[3] = bottomRight;

        for(size_t i = 0; i < vertices.size(); i++)
        {
            vertices[i].position -= Vector3(0.5f, 0.5f, 0.0f);
        }

        std::vector<GLuint> indices =
        {
            0, 1, 2, 
            1, 3, 2
        };

        SetScale(vertices, scale);
        Mesh mesh(vertices, indices, true);
        mesh.Generate();
        return mesh;
    }

    Mesh MeshGenerator::CreateSkybox(const Vector3 &scale)
    {
        std::vector<Vertex> vertices;
        vertices.resize(36);

        vertices[0].position = Vector3(-1.0f, 1.0f, -1.0f);
        vertices[1].position = Vector3(-1.0f, -1.0f, -1.0f);
        vertices[2].position = Vector3(1.0f, -1.0f, -1.0f);
        vertices[3].position = Vector3(1.0f, -1.0f, -1.0f);
        vertices[4].position = Vector3(1.0f, 1.0f, -1.0f);
        vertices[5].position = Vector3(-1.0f, 1.0f, -1.0f);

        vertices[6].position = Vector3(-1.0f, -1.0f, 1.0f);
        vertices[7].position = Vector3(-1.0f, -1.0f, -1.0f);
        vertices[8].position = Vector3(-1.0f, 1.0f, -1.0f);
        vertices[9].position = Vector3(-1.0f, 1.0f, -1.0f);
        vertices[10].position = Vector3(-1.0f, 1.0f, 1.0f);
        vertices[11].position = Vector3(-1.0f, -1.0f, 1.0f);

        vertices[12].position = Vector3(1.0f, -1.0f, -1.0f);
        vertices[13].position = Vector3(1.0f, -1.0f, 1.0f);
        vertices[14].position = Vector3(1.0f, 1.0f, 1.0f);
        vertices[15].position = Vector3(1.0f, 1.0f, 1.0f);
        vertices[16].position = Vector3(1.0f, 1.0f, -1.0f);
        vertices[17].position = Vector3(1.0f, -1.0f, -1.0f);

        vertices[18].position = Vector3(-1.0f, -1.0f, 1.0f);
        vertices[19].position = Vector3(-1.0f, 1.0f, 1.0f);
        vertices[20].position = Vector3(1.0f, 1.0f, 1.0f);
        vertices[21].position = Vector3(1.0f, 1.0f, 1.0f);
        vertices[22].position = Vector3(1.0f, -1.0f, 1.0f);
        vertices[23].position = Vector3(-1.0f, -1.0f, 1.0f);

        vertices[24].position = Vector3(-1.0f, 1.0f, -1.0f);
        vertices[25].position = Vector3(1.0f, 1.0f, -1.0f);
        vertices[26].position = Vector3(1.0f, 1.0f, 1.0f);
        vertices[27].position = Vector3(1.0f, 1.0f, 1.0f);
        vertices[28].position = Vector3(-1.0f, 1.0f, 1.0f);
        vertices[29].position = Vector3(-1.0f, 1.0f, -1.0f);

        vertices[30].position = Vector3(-1.0f, -1.0f, -1.0f);
        vertices[31].position = Vector3(-1.0f, -1.0f, 1.0f);
        vertices[32].position = Vector3(1.0f, -1.0f, -1.0f);
        vertices[33].position = Vector3(1.0f, -1.0f, -1.0f);
        vertices[34].position = Vector3(-1.0f, -1.0f, 1.0f);
        vertices[35].position = Vector3(1.0f, -1.0f, 1.0f);

        for(size_t i = 0; i < vertices.size(); i+=6)
        {
            Vertex v1 = vertices[i+0];
            Vertex v2 = vertices[i+1];
            Vertex v3 = vertices[i+2];
            Vertex v4 = vertices[i+3];
            Vertex v5 = vertices[i+4];
            Vertex v6 = vertices[i+5];

            v1.uv = Vector2(0.0f, 0.0f);
            v2.uv = Vector2(0.0f, 1.0f);
            v3.uv = Vector2(1.0f, 1.0f);
            v4.uv = Vector2(1.0f, 1.0f);
            v5.uv = Vector2(1.0f, 0.0f);
            v6.uv = Vector2(0.0f, 0.0f);

            vertices[i+0] = v1;
            vertices[i+1] = v2;
            vertices[i+2] = v3;
            vertices[i+3] = v4;
            vertices[i+4] = v5;
            vertices[i+5] = v6;
        }
        
        std::vector<GLuint> indices;
        Mesh mesh(vertices, indices, false);
        mesh.Generate();
        return mesh;
    }

    Mesh MeshGenerator::CreateSphere(const Vector3 &scale)
    {
        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;

        size_t sectorCount = 72;
        size_t stackCount = 24;
        float radius = 0.5f;

        float x, y, z, xy;                              // vertex position
        float lengthInv = 1.0f / radius;    // vertex normal
        float s, t;                                     // vertex texCoord

		const float PI = glm::pi<float>();

        float sectorStep = 2 * PI / sectorCount;
                
        float stackStep = PI / stackCount;
        float sectorAngle, stackAngle;

        for(size_t i = 0; i <= stackCount; ++i)
        {
			
            stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
            xy = radius* glm::cos(stackAngle);             // r * cos(u)
            z = radius* glm::sin(stackAngle);              // r * sin(u)

            // add (sectorCount+1) vertices per stack
            // the first and last vertices have same position and normal, but different tex coords
            for(size_t j = 0; j <= sectorCount; ++j)
            {
                Vertex v;

                sectorAngle = j * sectorStep;           // starting from 0 to 2pi

                // vertex position (x, y, z)
                x = xy * glm::cos(sectorAngle);             // r * cos(u) * cos(v)
                y = xy * glm::sin(sectorAngle);             // r * cos(u) * sin(v)          
                v.position = Vector3(x, y, z);

                // vertex tex coord (s, t) range between [0, 1]
                s = (float) j / sectorCount;
                t = (float) i / stackCount;          
                v.uv = Vector2(s, t);
                
                vertices.push_back(v);
            }
        }

        GLuint k1, k2;
        for(GLuint i = 0; i < stackCount; ++i)
        {
            k1 = i * (sectorCount + 1);     // beginning of current stack
            k2 = k1 + sectorCount + 1;      // beginning of next stack

            for(int j = 0; j<sectorCount; ++j, ++k1, ++k2)
            {
                // 2 triangles per sector excluding first and last stacks
                // k1 => k2 => k1+1
                if(i != 0)
                {
                    indices.push_back(k1);
                    indices.push_back(k2);
                    indices.push_back(k1 + 1);
                }

                // k1+1 => k2 => k2+1
                if(i != (stackCount-1))
                {
                    indices.push_back(k1 + 1);
                    indices.push_back(k2);
                    indices.push_back(k2 + 1);
                }
            }
        }

        SetScale(vertices, scale);
        Mesh mesh(vertices, indices, true);
        mesh.Generate();
        return mesh;
    }

    Mesh MeshGenerator::CreateHemisphere(const Vector3 &scale)
    {
        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;

        size_t sectorCount = 72;
        size_t stackCount = 24;
        float radius = 0.5f;

        float x, y, z, xy;                              // vertex position
        float lengthInv = 1.0f / radius;    // vertex normal
        float s, t;                                     // vertex texCoord

		const float PI = glm::pi<float>();

        float sectorStep = 2 * PI / sectorCount;
        
        sectorStep = PI / sectorCount;
        
        float stackStep = PI / stackCount;
        float sectorAngle, stackAngle;

        for(size_t i = 0; i <= stackCount; ++i)
        {
            stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
            xy = radius* glm::cos(stackAngle);             // r * cos(u)
            z = radius* glm::sin(stackAngle);              // r * sin(u)

            // add (sectorCount+1) vertices per stack
            // the first and last vertices have same position and normal, but different tex coords
            for(size_t j = 0; j <= sectorCount; ++j)
            {
                Vertex v;

                sectorAngle = j * sectorStep;           // starting from 0 to 2pi

                // vertex position (x, y, z)
                x = xy * glm::cos(sectorAngle);             // r * cos(u) * cos(v)
                y = xy * glm::sin(sectorAngle);             // r * cos(u) * sin(v)          
                v.position = Vector3(x, y, z);

                // vertex tex coord (s, t) range between [0, 1]
                s = (float) j / sectorCount;
                t = (float) i / stackCount;          
                v.uv = Vector2(s, t);
                
                vertices.push_back(v);
            }
        }

        GLuint k1, k2;
        for(GLuint i = 0; i < stackCount; ++i)
        {
            k1 = i * (sectorCount + 1);     // beginning of current stack
            k2 = k1 + sectorCount + 1;      // beginning of next stack

            for(int j = 0; j<sectorCount; ++j, ++k1, ++k2)
            {
                // 2 triangles per sector excluding first and last stacks
                // k1 => k2 => k1+1
                if(i != 0)
                {
                    indices.push_back(k1);
                    indices.push_back(k2);
                    indices.push_back(k1 + 1);
                }

                // k1+1 => k2 => k2+1
                if(i != (stackCount-1))
                {
                    indices.push_back(k1 + 1);
                    indices.push_back(k2);
                    indices.push_back(k2 + 1);
                }
            }
        }

        SetScale(vertices, scale);
        Mesh mesh(vertices, indices, true);
        mesh.Generate();
        return mesh;
    }

    Mesh MeshGenerator::CreateTerrain(uint32_t width, uint32_t height, const Vector3 &scale)
    {
        width += 1;
        height += 1;

        int32_t LOD = 0;

        int32_t meshSimplificationIncrement = (LOD == 0) ? 1 : LOD * 2;
        uint32_t verticesPerLine = (uint32_t)((width - 1) / meshSimplificationIncrement + 1);

        float topLeftX = 0;
        float topLeftZ = 0;

        uint32_t numVertices = verticesPerLine * height;
        uint32_t numIndices = (verticesPerLine - 1) * (verticesPerLine - 1) * 6;
        uint32_t vertexIndex = 0;
        int32_t triangleIndex = 0;

        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;

        vertices.resize(numVertices);
        indices.resize(numIndices);

        for (int y = 0; y < height; y += meshSimplificationIncrement)
        {
            for (int x = 0; x < width; x += meshSimplificationIncrement)
            {
                vertices[vertexIndex].position = Vector3(topLeftX + x, 0.0f, topLeftZ - y);
                vertices[vertexIndex].uv = Vector2((float)x / (width - 1.0f), (float)y / (height - 1.0f));

                if (x < width - 1 && y < height - 1)
                {
                    indices[triangleIndex + 0] = vertexIndex;
                    indices[triangleIndex + 1] = vertexIndex + verticesPerLine + 1;
                    indices[triangleIndex + 2] = vertexIndex + verticesPerLine;

                    indices[triangleIndex + 3] = vertexIndex + verticesPerLine + 1;
                    indices[triangleIndex + 4] = vertexIndex;
                    indices[triangleIndex + 5] = vertexIndex + 1;
                    triangleIndex += 6;
                }
                vertexIndex++;
            }
        }

        SetScale(vertices, scale);
        Mesh mesh(vertices, indices, true);
        mesh.Generate();
        return mesh;
    }

    Mesh MeshGenerator::CreateTerrain2(uint32_t width, uint32_t height, const Vector3 &scale)
    {
        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;

        // Step 1: Generate vertices (grid of height values)
        for (int z = 0; z < height; ++z) 
        {
            for (int x = 0; x < width; ++x) 
            {
                Vector3 position(x, 0, z);

                // UVs are just the normalized position
                Vector2 uv(float(x) / (width - 1), float(z) / (height - 1));

                // Create vertex
                vertices.push_back({ position, Vector3(0, 1, 0), uv });
            }
        }

        // Step 2: Generate indices (make triangles)
        for (int z = 0; z < height - 1; ++z) 
        {
            for (int x = 0; x < width - 1; ++x) 
            {
                // Create two triangles for each quad
                uint32_t topLeft = z * width + x;
                uint32_t topRight = z * width + (x + 1);
                uint32_t bottomLeft = (z + 1) * width + x;
                uint32_t bottomRight = (z + 1) * width + (x + 1);

                // First triangle (top-left, top-right, bottom-left)
                indices.push_back(topLeft);
                indices.push_back(bottomLeft);
                indices.push_back(topRight);

                // Second triangle (top-right, bottom-left, bottom-right)
                indices.push_back(topRight);
                indices.push_back(bottomLeft);
                indices.push_back(bottomRight);
            }
        }

        SetScale(vertices, scale);
        Mesh mesh(vertices, indices, true);
        mesh.Generate();
        return mesh;
    }

    Mesh MeshGenerator::CreateIcosahedron(const Vector3 &scale)
    {
        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;

        int subdivisions = 3;

        IcosahedronGenerator icosahedron;
        icosahedron.Initialize();
        icosahedron.Subdivide(subdivisions);

        auto &icoPolygons = icosahedron.GetPolygons();
        auto &icoVertices = icosahedron.GetVertices();
        uint32_t vertexCount = icoPolygons.size() * 3;
        indices.resize(vertexCount);

        vertices.resize(vertexCount);

        for (int i = 0; i < icoPolygons.size(); i++)
        {
            auto &poly = icoPolygons[i];

            indices[i * 3 + 0] = i * 3 + 0;
            indices[i * 3 + 1] = i * 3 + 1;
            indices[i * 3 + 2] = i * 3 + 2;

            vertices[i * 3 + 0].position = icoVertices[poly.vertices[0]];
            vertices[i * 3 + 1].position = icoVertices[poly.vertices[1]];
            vertices[i * 3 + 2].position = icoVertices[poly.vertices[2]];

            vertices[i * 3 + 0].normal = icoVertices[poly.vertices[0]];
            vertices[i * 3 + 1].normal = icoVertices[poly.vertices[1]];
            vertices[i * 3 + 2].normal = icoVertices[poly.vertices[2]];
        }

        SetScale(vertices, scale);
        Mesh mesh(vertices, indices, true);
        mesh.Generate();
        return mesh;
    }


    std::vector<Polygon> &IcosahedronGenerator::GetPolygons()
    {
        return polygons;
    }

    std::vector<Vector3> &IcosahedronGenerator::GetVertices()
    {
        return vertices;
    }

    void IcosahedronGenerator::Initialize()
    {
        // An icosahedron has 12 vertices, and
        // since it's completely symmetrical the
        // formula for calculating them is kind of
        // symmetrical too:

        float t = (1.0f + glm::sqrt(5.0f)) / 2.0f;

        vertices.push_back(Vector3f::Normalize(Vector3(-1, t, 0)));
        vertices.push_back(Vector3f::Normalize(Vector3(1, t, 0)));
        vertices.push_back(Vector3f::Normalize(Vector3(-1, -t, 0)));
        vertices.push_back(Vector3f::Normalize(Vector3(1, -t, 0)));
        vertices.push_back(Vector3f::Normalize(Vector3(0, -1, t)));
        vertices.push_back(Vector3f::Normalize(Vector3(0, 1, t)));
        vertices.push_back(Vector3f::Normalize(Vector3(0, -1, -t)));
        vertices.push_back(Vector3f::Normalize(Vector3(0, 1, -t)));
        vertices.push_back(Vector3f::Normalize(Vector3(t, 0, -1)));
        vertices.push_back(Vector3f::Normalize(Vector3(t, 0, 1)));
        vertices.push_back(Vector3f::Normalize(Vector3(-t, 0, -1)));
        vertices.push_back(Vector3f::Normalize(Vector3(-t, 0, 1)));

        // And here's the formula for the 20 sides,
        // referencing the 12 vertices we just created.
        polygons.push_back(Polygon(0, 11, 5));
        polygons.push_back(Polygon(0, 5, 1));
        polygons.push_back(Polygon(0, 1, 7));
        polygons.push_back(Polygon(0, 7, 10));
        polygons.push_back(Polygon(0, 10, 11));
        polygons.push_back(Polygon(1, 5, 9));
        polygons.push_back(Polygon(5, 11, 4));
        polygons.push_back(Polygon(11, 10, 2));
        polygons.push_back(Polygon(10, 7, 6));
        polygons.push_back(Polygon(7, 1, 8));
        polygons.push_back(Polygon(3, 9, 4));
        polygons.push_back(Polygon(3, 4, 2));
        polygons.push_back(Polygon(3, 2, 6));
        polygons.push_back(Polygon(3, 6, 8));
        polygons.push_back(Polygon(3, 8, 9));
        polygons.push_back(Polygon(4, 9, 5));
        polygons.push_back(Polygon(2, 4, 11));
        polygons.push_back(Polygon(6, 2, 10));
        polygons.push_back(Polygon(8, 6, 7));
        polygons.push_back(Polygon(9, 8, 1));
    }

    void IcosahedronGenerator::Subdivide(uint32_t recursions)
    {
        std::unordered_map<uint32_t,uint32_t> midPointCache;

        for (size_t i = 0; i < recursions; i++)
        {
            std::vector<Polygon> newPolys;

            for (auto &poly : polygons)
            {
                uint32_t a = poly.vertices[0];
                uint32_t b = poly.vertices[1];
                uint32_t c = poly.vertices[2];
                // Use GetMidPointIndex to either create a
                // new vertex between two old vertices, or
                // find the one that was already created.
                uint32_t ab = GetMidPointIndex(midPointCache, a, b);
                uint32_t bc = GetMidPointIndex(midPointCache, b, c);
                uint32_t ca = GetMidPointIndex(midPointCache, c, a);
                // Create the four new polygons using our original
                // three vertices, and the three new midpoints.
                newPolys.push_back(Polygon(a, ab, ca));
                newPolys.push_back(Polygon(b, bc, ab));
                newPolys.push_back(Polygon(c, ca, bc));
                newPolys.push_back(Polygon(ab, bc, ca));
            }
            // Replace all our old polygons with the new set of
            // subdivided ones.
            polygons = newPolys;
        }
    }

    uint32_t IcosahedronGenerator::GetMidPointIndex(std::unordered_map<uint32_t, uint32_t> &cache, uint32_t indexA, uint32_t indexB)
    {
        // We create a key out of the two original indices
        // by storing the smaller index in the upper two bytes
        // of an integer, and the larger index in the lower two
        // bytes. By sorting them according to whichever is smaller
        // we ensure that this function returns the same result
        // whether you call
        // GetMidPointIndex(cache, 5, 9)
        // or...
        // GetMidPointIndex(cache, 9, 5)
        int smallerIndex = glm::min(indexA, indexB);
        int greaterIndex = glm::max(indexA, indexB);
        int key = (smallerIndex << 16) + greaterIndex;
        // If a midpoint is already defined, just return it.
        int ret;
        if (cache.count(key) > 0)
        {
            uint32_t ret = cache[key];
            return ret;
        }
        // If we're here, it's because a midpoint for these two
        // vertices hasn't been created yet. Let's do that now!
        Vector3 p1 = vertices[indexA];
        Vector3 p2 = vertices[indexB];
        Vector3 middle = Vector3f::Normalize(Vector3f::Lerp(p1, p2, 0.5f));

        ret = vertices.size();
        vertices.push_back(middle);

        cache[key] = ret;
        return ret;
    }
}