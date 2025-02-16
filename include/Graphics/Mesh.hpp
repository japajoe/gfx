#ifndef GFX_MESH_HPP
#define GFX_MESH_HPP

#include "Vertex.hpp"
#include "BoundingBox.hpp"
#include "Buffers/ElementBufferObject.hpp"
#include "Buffers/VertexArrayObject.hpp"
#include "Buffers/VertexBufferObject.hpp"
#include "../System/Numerics/Vector3.hpp"
#include "../System/Numerics/Vector4.hpp"
#include "../Core/Object.hpp"
#include <vector>
#include <cstdint>
#include <string>
#include <unordered_map>

namespace GFX
{
    class Mesh
    {
    public:
        Mesh();
        Mesh(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices, bool calculateNormals);
        Mesh(const Mesh &other);
        Mesh(Mesh &&other) noexcept;
        Mesh& operator=(const Mesh &other);
        Mesh& operator=(Mesh &&other) noexcept;
        std::vector<Vertex> &GetVertices();
        std::vector<uint32_t> &GetIndices();
        size_t GetVerticesCount() const;
        size_t GetIndicesCount() const;
        VertexArrayObject *GetVAO();
        VertexBufferObject *GetVBO();
        ElementBufferObject *GetEBO();
        BoundingBox GetBounds() const;
        void SetName(const std::string &name);
        std::string GetName() const;
        void Generate();
        void Delete();
        void RecalculateNormals();
    private:
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        size_t sizeOfVertices;
        size_t sizeOfIndices;
        VertexArrayObject VAO;
        VertexBufferObject VBO;
        ElementBufferObject EBO;
        BoundingBox bounds;
        std::string name;
        Vector3 SurfaceNormalFromIndices(int32_t indexA, int32_t indexB, int32_t indexC);
    };

    class MeshGenerator
    {
    public:
        static Mesh Combine(const std::vector<Mesh*> &meshes);
        static Mesh CreateCapsule(const Vector3 &scale);
        static Mesh CreateCube(const Vector3 &scale);
        static Mesh CreateCylinder(const Vector3 &scale);
        static Mesh CreatePlane(const Vector3 &scale);
        static Mesh CreateSkybox(const Vector3 &scale);
        static Mesh CreateQuad(const Vector3 &scale);
        static Mesh CreateSphere(const Vector3 &scale);
        static Mesh CreateHemisphere(const Vector3 &scale);
        static Mesh CreateTerrain(uint32_t width, uint32_t height, const Vector3 &scale);
        static Mesh CreateTerrain2(uint32_t width, uint32_t height, const Vector3 &scale);
        static Mesh CreateIcosahedron(const Vector3 &scale);
    private:
        static void SetScale(std::vector<Vertex> &vertices, const Vector3 &scale);
    };

    struct Polygon
    {
        std::vector<uint32_t> vertices;

        Polygon (uint32_t a, uint32_t b, uint32_t c)
        {
            vertices = { a, b, c };
        }
    };

    class IcosahedronGenerator
    {
    public:
        std::vector<Polygon> &GetPolygons();
        std::vector<Vector3> &GetVertices();
        void Initialize();
        void Subdivide(uint32_t recursions);
        uint32_t GetMidPointIndex(std::unordered_map<uint32_t, uint32_t> &cache, uint32_t indexA, uint32_t indexB);
    private:
        std::vector<Polygon> polygons;
        std::vector<Vector3> vertices;
    };
}

#endif