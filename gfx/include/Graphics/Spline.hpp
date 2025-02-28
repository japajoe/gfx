#ifndef GFX_SPLINE_HPP
#define GFX_SPLINE_HPP

#include "../System/Numerics/Vector2.hpp"
#include "../System/Numerics/Vector3.hpp"
#include "../System/Numerics/Quaternion.hpp"
#include "../System/Numerics/Matrix4.hpp"
#include "Mesh.hpp"
#include <vector>
#include <cstdlib>
#include <cstdint>

namespace GFX
{
    class ExtrusionShapeVertex
    {
    public:
        Vector2 point;
        Vector3 normal;
        float u;

        ExtrusionShapeVertex()
        {
            
        }

        ExtrusionShapeVertex(const Vector2 &point, float u)
        {
            this->point = point;
            this->u = u;
        }
    };

    class Edge
    {
    public:
        // The index to each vertex
        std::vector<uint32_t> vertexIndex;
        // The index into the face.
        // (faceindex[0] == faceindex[1] means the edge connects to only one triangle)
        std::vector<uint32_t> faceIndex;
        Edge();
        static std::vector<Edge> BuildManifoldEdges(Mesh *mesh);
        static std::vector<Edge> BuildEdges(size_t vertexCount, const std::vector<uint32_t> &triangleArray);
    };

    class ExtrusionShape
    {
    public:
        std::vector<ExtrusionShapeVertex> vertices;
        std::vector<uint32_t> lineIndices;
        size_t GetVertexCount() const;
        size_t GetLineCount() const;
        float CalculateUSpan();
        bool Load(const std::string &filepath);
        bool Load(Mesh *mesh);
        bool Save(const std::string &filepath);
    };

    class OrientedPoint
    {
    public:
        Vector3 position;
        Quaternion rotation;
        Matrix4 model;
        OrientedPoint();
        OrientedPoint(const Vector3 &position, const Quaternion &rotation);
        void ComputeModelMatrix();
        Vector3 LocalToWorldPosition(const Vector3 &localPosition);
        Vector3 LocalToWorldPosition(const Vector2 &localPosition);
        Vector3 LocalToWorldVector(const Vector3 &localPosition);
        Vector3 LocalToWorldVector(const Vector2 &localPosition);
    };

    class Bezier
    {
    private:
        std::vector<OrientedPoint> controlPoints;
    public:
        Bezier();
        std::vector<OrientedPoint> &GetControlPoints();
        bool GetControlPoint(size_t index, OrientedPoint &point);
        Vector3 GetPoint(float t) const;
        Quaternion GetOrientation(float t) const;
        Vector3 GetTangent(float t) const;
        OrientedPoint GetOrientedPoint(float t) const;
    };

	class Spline
	{
	private:
		ExtrusionShape extrusionShape;
		Mesh mesh;
		float percentage;
		bool canRender;
		bool viewHandles;
		bool viewSpline;
		bool viewShape;
		bool isRegistered;
		int ringCount;
		int32_t conformWidth;
		Bezier curve;
		float GetApproximateLengthOfCurve(int numSteps = 8);
	public:
		Spline();
		void SetShape(const ExtrusionShape &shape);
		ExtrusionShape &GetShape();
		void GenerateMesh();
		void RenderSpline();
		void RenderShape();
		Mesh *GetMesh();
        Bezier *GetBezier();
	};
}

#endif