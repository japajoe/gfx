#include "Spline.hpp"
#include "../System/IO/File.hpp"
#include "../System/IO/BinaryStream.hpp"
#include "../Core/Camera.hpp"
#include "../Core/Debug.hpp"

namespace GFX
{
    size_t ExtrusionShape::GetVertexCount() const
    {
        return vertices.size();
    }

    size_t ExtrusionShape::GetLineCount() const
    {
        return lineIndices.size();
    }

    float ExtrusionShape::CalculateUSpan()
    {
        float distance = 0.0f;

        for(size_t i = 0; i < GetLineCount(); i+=2) 
        {
            Vector2 uA = vertices[lineIndices[i]].point;
            Vector2 uB = vertices[lineIndices[i+1]].point;
            distance += Vector2f::Distance(uA, uB);
        }

        return distance;
    }

    bool ExtrusionShape::Load(const std::string &filepath)
    {
        if(!File::Exists(filepath))
            return false;
        
        auto bytes = File::ReadAllBytes(filepath);

        BinaryStream stream(bytes.data(), bytes.size(), bytes.size());

        uint32_t vertexCount = stream.ReadUInt32();
        
        vertices.resize(vertexCount);

        for(size_t i = 0; i < vertexCount; i++)
        {
            float x = stream.ReadFloat();
            float y = stream.ReadFloat() * -1.0f;
            float u = stream.ReadFloat();
            vertices[i] = ExtrusionShapeVertex(Vector2(x, y), u);
        }

        uint32_t indiceCount = stream.ReadUInt32();
        lineIndices.resize(indiceCount);

        for(size_t i = 0; i < indiceCount; i++)
        {
            uint32_t index = stream.ReadUInt32();
            lineIndices[i] = index;
        }

        return true;
    }

    bool ExtrusionShape::Load(Mesh *mesh)
    {
        auto edges = Edge::BuildManifoldEdges(mesh);

        if(edges.size() < 1)
            return false;
        
        size_t vertexCount = edges.size() * 2;
        size_t indiceCount = edges.size() * 2;

        vertices.resize(vertexCount);
        lineIndices.resize(indiceCount);

        auto &mVertices = mesh->GetVertices();
        uint32_t index = 0;
        
        for(size_t i = 0; i < edges.size(); i++)
        {
            Vertex p1 = mVertices[edges[i].vertexIndex[0]];
            Vertex p2 = mVertices[edges[i].vertexIndex[1]];
            
            vertices[index+0] = ExtrusionShapeVertex(Vector2(p1.position.x, p1.position.y), p1.uv.x);
            vertices[index+1] = ExtrusionShapeVertex(Vector2(p2.position.x, p2.position.y), p2.uv.x);

            lineIndices[index+0] = index + 0;
            lineIndices[index+1] = index + 1;
            index += 2;
        }

        return true;
    }

    bool ExtrusionShape::Save(const std::string &filepath)
    {
        if(vertices.size() == 0)
            return false;

        if(vertices.size() % 2 != 0)
            return false;

        size_t neededSpace =    sizeof(uint32_t) + //num vertices
                                (vertices.size() * sizeof(Vector3)) + //vertices and U's
                                sizeof(uint32_t) + //num indices
                                (lineIndices.size() * sizeof(uint32_t)); //indices

        std::vector<unsigned char> bytes;
        bytes.resize(neededSpace);

        BinaryStream stream(bytes.data(), bytes.size());
        stream.Write(static_cast<uint32_t>(vertices.size()));

        for(size_t i = 0; i < vertices.size(); i++)
        {
            stream.Write(vertices[i].point.x);
            stream.Write(vertices[i].point.y);
            stream.Write(vertices[i].u);
        }

        stream.Write(static_cast<uint32_t>(lineIndices.size()));

        for(size_t i = 0; i < lineIndices.size(); i++)
        {
            stream.Write(lineIndices[i]);
        }

        File::WriteAllBytes(filepath, bytes.data(), bytes.size());
        return true;
    }

    Edge::Edge()
    {
        vertexIndex.resize(2);
        faceIndex.resize(2);
    }

    // Builds an array of edges that connect to only one triangle.
    // In other words, the outline of the mesh 
    std::vector<Edge> Edge::BuildManifoldEdges(Mesh *mesh)
    {
        // Build a edge list for all unique edges in the mesh
        std::vector<Edge> edges = BuildEdges(mesh->GetVerticesCount(), mesh->GetIndices());

        // We only want edges that connect to a single triangle
        std::vector<Edge> culledEdges;

        for(size_t i = 0; i < edges.size(); i++)
        {
            if (edges[i].faceIndex[0] == edges[i].faceIndex[1])
            {
                culledEdges.push_back(edges[i]);
            }
        }

        return culledEdges;
    }

    // Builds an array of unique edges
    // This requires that your mesh has all vertices welded. However on import, Unity has to split
    // vertices at uv seams and normal seams. Thus for a mesh with seams in your mesh you
    // will get two edges adjoining one triangle.
    // Often this is not a problem but you can fix it by welding vertices 
    // and passing in the triangle array of the welded vertices.
    std::vector<Edge> Edge::BuildEdges(size_t vertexCount, const std::vector<uint32_t> &triangleArray)
    {
        int maxEdgeCount = triangleArray.size();
        std::vector<int32_t> firstEdge;
        firstEdge.resize(vertexCount + maxEdgeCount);
        int nextEdge = vertexCount;
        int triangleCount = triangleArray.size() / 3;

        for(int a = 0; a < vertexCount; a++)
        {
            firstEdge[a] = -1;
        }

        // First pass over all triangles. This finds all the edges satisfying the
        // condition that the first vertex index is less than the second vertex index
        // when the direction from the first vertex to the second vertex represents
        // a counterclockwise winding around the triangle to which the edge belongs.
        // For each edge found, the edge index is stored in a linked list of edges
        // belonging to the lower-numbered vertex index i. This allows us to quickly
        // find an edge in the second pass whose higher-numbered vertex index is i.
        std::vector<Edge> edgeArray;
        edgeArray.resize(maxEdgeCount);

        int edgeCount = 0;
        for (int a = 0; a < triangleCount; a++)
        {
            int i1 = triangleArray[a * 3 + 2];
            for (int b = 0; b < 3; b++)
            {
                int i2 = triangleArray[a * 3 + b];
                if (i1 < i2)
                {
                    Edge newEdge;
                    newEdge.vertexIndex[0] = i1;
                    newEdge.vertexIndex[1] = i2;
                    newEdge.faceIndex[0] = a;
                    newEdge.faceIndex[1] = a;
                    edgeArray[edgeCount] = newEdge;

                    int edgeIndex = firstEdge[i1];
                    if (edgeIndex == -1)
                    {
                        firstEdge[i1] = edgeCount;
                    }
                    else
                    {
                        while (true)
                        {
                            int index = firstEdge[nextEdge + edgeIndex];
                            if (index == -1)
                            {
                                firstEdge[nextEdge + edgeIndex] = edgeCount;
                                break;
                            }

                            edgeIndex = index;
                        }
                    }

                    firstEdge[nextEdge + edgeCount] = -1;
                    edgeCount++;
                }

                i1 = i2;
            }
        }

        // Second pass over all triangles. This finds all the edges satisfying the
        // condition that the first vertex index is greater than the second vertex index
        // when the direction from the first vertex to the second vertex represents
        // a counterclockwise winding around the triangle to which the edge belongs.
        // For each of these edges, the same edge should have already been found in
        // the first pass for a different triangle. Of course we might have edges with only one triangle
        // in that case we just add the edge here
        // So we search the list of edges
        // for the higher-numbered vertex index for the matching edge and fill in the
        // second triangle index. The maximum number of comparisons in this search for
        // any vertex is the number of edges having that vertex as an endpoint.

        for (int a = 0; a < triangleCount; a++)
        {
            int i1 = triangleArray[a * 3 + 2];
            for (int b = 0; b < 3; b++)
            {
                int i2 = triangleArray[a * 3 + b];
                if (i1 > i2)
                {
                    bool foundEdge = false;
                    for (int edgeIndex = firstEdge[i2]; edgeIndex != -1; edgeIndex = firstEdge[nextEdge + edgeIndex])
                    {
                        Edge edge = edgeArray[edgeIndex];
                        if ((edge.vertexIndex[1] == i1) && (edge.faceIndex[0] == edge.faceIndex[1]))
                        {
                            edgeArray[edgeIndex].faceIndex[1] = a;
                            foundEdge = true;
                            break;
                        }
                    }

                    if (!foundEdge)
                    {
                        Edge newEdge;
                        newEdge.vertexIndex[0] = i1;
                        newEdge.vertexIndex[1] = i2;
                        newEdge.faceIndex[0] = a;
                        newEdge.faceIndex[1] = a;
                        edgeArray[edgeCount] = newEdge;
                        edgeCount++;
                    }
                }

                i1 = i2;
            }
        }

        std::vector<Edge> compactedEdges;
        compactedEdges.resize(edgeCount);
        for (int e = 0; e < edgeCount; e++)
            compactedEdges[e] = edgeArray[e];

        return compactedEdges;
    }

    OrientedPoint::OrientedPoint()
    {
        position = Vector3f::Zero();
        rotation = Quaternionf::Identity();
        ComputeModelMatrix();
    }

    OrientedPoint::OrientedPoint(const Vector3 &position, const Quaternion &rotation)
    {
        this->position = position;
        this->rotation = rotation;
        ComputeModelMatrix();
    }

    void OrientedPoint::ComputeModelMatrix()
    {
        Matrix4 t = Matrix4f::CreateTranslation(position.x, position.y, position.z);
        Matrix4 r = Matrix4f::CreateRotation(rotation.x, rotation.y, rotation.z, rotation.w);
        Matrix4 s = Matrix4f::CreateScale(1.0f, 1.0f, 1.0f);
        model = t * r * s;
    }

    Vector3 OrientedPoint::LocalToWorldPosition(const Vector3 &localPosition)
    {
        return position + rotation * localPosition;
    }

    Vector3 OrientedPoint::LocalToWorldPosition(const Vector2 &localPosition)
    {
        return position + rotation * Vector3(localPosition, 0.0f);
    }

    Vector3 OrientedPoint::LocalToWorldVector(const Vector3 &localPosition)
    {
        return rotation * localPosition;
    }

    Vector3 OrientedPoint::LocalToWorldVector(const Vector2 &localPosition)
    {
        return rotation * Vector3(localPosition, 0.0f);
    }

    Bezier::Bezier()
    {
        controlPoints.resize(4);
        for(size_t i = 0; i < controlPoints.size(); i++)
        {
            controlPoints[i].position = Vector3f::Zero();
            controlPoints[i].rotation = Quaternionf::Identity();
            controlPoints[i].ComputeModelMatrix();
        }
    }

    std::vector<OrientedPoint> &Bezier::GetControlPoints()
    {
        return controlPoints;
    }

    bool Bezier::GetControlPoint(size_t index, OrientedPoint &point)
    {
        if(index >= controlPoints.size())
            return false;
        
        point = controlPoints[index];
        return true;
    }

    Vector3 Bezier::GetPoint(float t) const
    {
        Vector3 p0 = controlPoints[0].position;
        Vector3 p1 = controlPoints[1].position;
        Vector3 p2 = controlPoints[2].position;
        Vector3 p3 = controlPoints[3].position;

        Vector3 a = Vector3f::Lerp(p0, p1, t);
        Vector3 b = Vector3f::Lerp(p1, p2, t);
        Vector3 c = Vector3f::Lerp(p2, p3, t);

        Vector3 d = Vector3f::Lerp(a, b, t);
        Vector3 e = Vector3f::Lerp(b, c, t);

        return Vector3f::Lerp(d, e, t);
    }
    
    Quaternion Bezier::GetOrientation(float t) const
    {
        Vector3 tangent = GetTangent(t);
        return Quaternionf::LookRotation(tangent);
    }

    Vector3 Bezier::GetTangent(float t) const
    {
        Vector3 p0 = controlPoints[0].position;
        Vector3 p1 = controlPoints[1].position;
        Vector3 p2 = controlPoints[2].position;
        Vector3 p3 = controlPoints[3].position;

        Vector3 a = Vector3f::Lerp(p0, p1, t);
        Vector3 b = Vector3f::Lerp(p1, p2, t);
        Vector3 c = Vector3f::Lerp(p2, p3, t);

        Vector3 d = Vector3f::Lerp(a, b, t);
        Vector3 e = Vector3f::Lerp(b, c, t);

        return Vector3f::Normalize(e - d);
    }

    OrientedPoint Bezier::GetOrientedPoint(float t) const
    {
        Vector3 p0 = controlPoints[0].position;
        Vector3 p1 = controlPoints[1].position;
        Vector3 p2 = controlPoints[2].position;
        Vector3 p3 = controlPoints[3].position;

        Vector3 a = Vector3f::Lerp(p0, p1, t);
        Vector3 b = Vector3f::Lerp(p1, p2, t);
        Vector3 c = Vector3f::Lerp(p2, p3, t);

        Vector3 d = Vector3f::Lerp(a, b, t);
        Vector3 e = Vector3f::Lerp(b, c, t);

        Vector3 position = Vector3f::Lerp(d, e, t);
        Vector3 tangent = Vector3f::Normalize(e - d);
        Quaternion rotation = Quaternionf::LookRotation(tangent);
        return OrientedPoint(position, rotation);
    }

    Spline::Spline()
    {
        canRender = false;
        viewHandles = true;
        viewSpline = true;
        viewShape = false;
        isRegistered = false;
        ringCount = 24;
        percentage = 0.0f;
        conformWidth = 4;

        auto &controlPoints = curve.GetControlPoints();

        controlPoints[0] = OrientedPoint(Vector3f::Zero(), Quaternionf::Identity());
        controlPoints[1] = OrientedPoint(Vector3(0, 0, -25), Quaternionf::Identity());
        controlPoints[2] = OrientedPoint(Vector3(0, 0, -50), Quaternionf::Identity());
        controlPoints[3] = OrientedPoint(Vector3(0, 0, -75), Quaternionf::Identity());

        auto &vertices = extrusionShape.vertices;
        auto &indices = extrusionShape.lineIndices;

        vertices.push_back(ExtrusionShapeVertex(Vector2(-6.000000, 0.150000), 0.000000));
        vertices.push_back(ExtrusionShapeVertex(Vector2(-4.000000, 0.150000), 0.150000));
        vertices.push_back(ExtrusionShapeVertex(Vector2(-4.000000, 0.150000), 0.000000));
        vertices.push_back(ExtrusionShapeVertex(Vector2(-4.000000, 0.000000), 0.150000));
        vertices.push_back(ExtrusionShapeVertex(Vector2(-4.000000, 0.000000), 0.200000));
        vertices.push_back(ExtrusionShapeVertex(Vector2(4.000000, 0.000000), 0.800000));
        vertices.push_back(ExtrusionShapeVertex(Vector2(4.000000, 0.000000), 0.000000));
        vertices.push_back(ExtrusionShapeVertex(Vector2(4.000000, 0.150000), 0.150000));
        vertices.push_back(ExtrusionShapeVertex(Vector2(4.000000, 0.150000), 0.850000));
        vertices.push_back(ExtrusionShapeVertex(Vector2(6.000000, 0.150000), 1.000000));
        indices.push_back(0);
        indices.push_back(1);
        indices.push_back(2);
        indices.push_back(3);
        indices.push_back(4);
        indices.push_back(5);
        indices.push_back(6);
        indices.push_back(7);
        indices.push_back(8);
        indices.push_back(9);
    }

    void Spline::RenderSpline()
    {
        float t = 1.0f / ringCount;        

        for(int i = 0 ; i < ringCount; i++)
        {
            OrientedPoint p1 = curve.GetOrientedPoint(t * i);
            OrientedPoint p2 = curve.GetOrientedPoint(t * (i + 1));
            Debug::DrawLine(p1.position, p2.position, Color::Red());
        }
    }

    void Spline::RenderShape()
    {
        Camera *camera = Camera::GetMain();

        OrientedPoint testPoint = curve.GetOrientedPoint(percentage);

        for(size_t i = 0; i < extrusionShape.GetLineCount(); i+=2)
        {
            size_t indexA = extrusionShape.lineIndices[i];
            size_t indexB = extrusionShape.lineIndices[i+1];
            Vector3 p1 = testPoint.LocalToWorldPosition(extrusionShape.vertices[indexA].point);
            Vector3 p2 = testPoint.LocalToWorldPosition(extrusionShape.vertices[indexB].point);
            Debug::DrawLine(p1, p2, Color::Red());
        }
    }

    void Spline::GenerateMesh()
    {
        //Vertices
        std::vector<Vertex> vertices;

        float curveLength = GetApproximateLengthOfCurve();
        float uSpan = extrusionShape.CalculateUSpan();

        for(size_t i = 0; i < ringCount; i++)
        {
            float t = ((float)i / (ringCount - 1.0f));
            OrientedPoint p = curve.GetOrientedPoint(t);

            for(size_t j = 0; j < extrusionShape.vertices.size(); j++)
            {
                Vertex v;
                v.position = p.LocalToWorldPosition(extrusionShape.vertices[j].point);
                v.normal = Vector3f::Zero();
                v.uv = Vector2(extrusionShape.vertices[j].u, t * curveLength / uSpan);
                vertices.push_back(v);
            }
        }

        //Triangles
        std::vector<uint32_t> indices;
        for(size_t ring = 0; ring < ringCount - 1; ring++)
        {
            uint32_t rootIndex = ring * extrusionShape.GetVertexCount();
            uint32_t rootIndexNext = (ring + 1) * extrusionShape.GetVertexCount();

            for(size_t line = 0; line < extrusionShape.GetLineCount(); line+=2)
            {
                uint32_t lineIndexA = extrusionShape.lineIndices[line];
                uint32_t lineIndexB = extrusionShape.lineIndices[line+1];
                uint32_t currentA = rootIndex + lineIndexA;
                uint32_t currentB = rootIndex + lineIndexB;
                uint32_t nextA = rootIndexNext + lineIndexA;
                uint32_t nextB = rootIndexNext + lineIndexB;

                indices.push_back(currentA);
                indices.push_back(nextA);
                indices.push_back(nextB);

                indices.push_back(currentA);
                indices.push_back(nextB);
                indices.push_back(currentB);
            }
        }

        auto &mVertices = mesh.GetVertices();
        auto &mIndices = mesh.GetIndices();

        mVertices.clear();
        mIndices.clear();

        for(size_t i = 0; i < vertices.size(); i++)
        {
            // Vertex v;
            // v.position = vertices[i];
            // v.normal = Vector3f::Zero();
            // v.uv = Vector2(0, 0);
            // mVertices.push_back(v);
            mVertices.push_back(vertices[i]);
        }

        for(size_t i = 0; i < indices.size(); i++)
        {
            mIndices.push_back(indices[i]);
        }

		mesh.Generate();
        mesh.RecalculateNormals();
    }

    float Spline::GetApproximateLengthOfCurve(int numSteps)
    {
        float length = 0;

        for(size_t i = 0; i < numSteps; i++)
        {
            float t1 = (float)i / (numSteps - 1);
            float t2 = (float)(i+1) / (numSteps - 1);

            Vector3 p1 = curve.GetPoint(t1);
            Vector3 p2 = curve.GetPoint(t2);
            length += Vector3f::Distance(p1, p2);
        }

        return length;
    }

    void Spline::SetShape(const ExtrusionShape &shape)
    {
        this->extrusionShape = shape;
        GenerateMesh();
    }

    ExtrusionShape &Spline::GetShape()
    {
        return extrusionShape;
    }

	Mesh *Spline::GetMesh()
	{
		return &mesh;
	}

    Bezier *Spline::GetBezier()
    {
        return &curve;
    }
}