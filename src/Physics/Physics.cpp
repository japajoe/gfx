#include "Physics.hpp"
#include "../Core/Debug.hpp"
#include "../Core/GameObject.hpp"
#include "../Core/Transform.hpp"
#include "../Core/GameBehaviour.hpp"
#include "../Core/Time.hpp"
#include "../System/Numerics/Vector3.hpp"
#include "../System/Numerics/Matrix4.hpp"
#include "../Graphics/Graphics.hpp"
#include "../Graphics/Mesh.hpp"
#include "../Graphics/Renderers/Renderer.hpp"
#include "../External/glm/glm.hpp"
#include <cstdint>

namespace GFX
{
	static constexpr float FloatMinValue = -3.4028235E38F;
	static constexpr float FloatMaxValue = 3.4028235E38F;

    struct TriangleIntersection
    {
        void *userData = nullptr;
        Transform *transform = nullptr;
        int32_t triangleIndex1 = -1;
        int32_t triangleIndex2 = -1;
        int32_t triangleIndex3 = -1;
        float lastPos = FloatMaxValue;
        Vector3 normal;
    };

    bool Physics::Raycast(const Ray &ray, RaycastHit &hit, Layer layerMask)
    {
        return Raycast(ray.origin, ray.direction, ray.length, hit, layerMask);
    }

	bool Physics::Raycast(const Vector3 &origin, const Vector3 &direction, float maxDistance, RaycastHit &hit, Layer layerMask)
	{
		if(BoxTest(origin, direction, maxDistance, hit, layerMask))
		{
			Renderer *renderer = reinterpret_cast<Renderer*>(hit.userData);
			
			if(renderer == nullptr)
				return false;

			if(!renderer->GetGameObject()->GetIsActive())
				return false;
			
			TriangleIntersection intersection;
			intersection.transform = nullptr;
			intersection.triangleIndex1 = -1;
			intersection.lastPos = std::numeric_limits<float>::max();
			float lastPos = std::numeric_limits<float>::max();
			uint32_t ignoreRaycast = static_cast<uint32_t>(Layer_IgnoreRaycast);

            uint32_t layer = static_cast<uint32_t>(renderer->GetGameObject()->GetLayer());

            if(layer > 0)
            {
                if(layer & layerMask || layer & ignoreRaycast)
                    return false;
            }

            Transform *transform = renderer->GetTransform();
            Matrix4 transformation = transform->GetModelMatrix();
            Ray ray(transform->WorldToLocal(origin), transform->WorldToLocalVector(direction), FloatMaxValue);
			Mesh *mesh = nullptr;
			size_t meshIndex = 0;

			while((mesh = renderer->GetMesh(meshIndex++)) != nullptr)
            {
                auto &indices = mesh->GetIndices();
                
                if(indices.size() == 0)
                    continue;

                auto bounds = mesh->GetBounds();

                float distance = 0;
                
                if(!bounds.Intersects(ray, distance))
                    continue;
                
                auto &vertices = mesh->GetVertices();

                for(size_t j = 0; j < indices.size() / 3; j++)
                {
                    float currIntersectionPos = 0.0f;

                    Vector3 v1 = vertices[indices[j * 3]].position;
                    Vector3 v2 = vertices[indices[j * 3 + 1]].position;
                    Vector3 v3 = vertices[indices[j * 3 + 2]].position;

                    Vector4 v1t = transformation * Vector4(v1.x, v1.y, v1.z, 1.0f);
                    Vector4 v2t = transformation * Vector4(v2.x, v2.y, v2.z, 1.0f);
                    Vector4 v3t = transformation * Vector4(v3.x, v3.y, v3.z, 1.0f);

                    v1 = Vector3(v1t.x, v1t.y, v1t.z);
                    v2 = Vector3(v2t.x, v2t.y, v2t.z);
                    v3 = Vector3(v3t.x, v3t.y, v3t.z);

                    if (RayIntersectsTriangle(origin, direction, v1, v2, v3, currIntersectionPos))
                    {
                        if (currIntersectionPos < intersection.lastPos)
                        {
                            intersection.lastPos = currIntersectionPos;
                            intersection.triangleIndex1 = indices[j*3];
                            intersection.triangleIndex2 = indices[j*3+1];
                            intersection.triangleIndex3 = indices[j*3+2];
                            intersection.transform = renderer->GetTransform();
                            hit.normal = SurfaceNormalFromIndices(v1, v2, v3);
                        }
                    }
                }
            }

			if(intersection.triangleIndex1 >= 0)
			{
				float totalDistance = Vector3f::Distance(origin, origin + (direction * intersection.lastPos));

				if(totalDistance <= maxDistance)
				{
					hit.point = origin + (direction * intersection.lastPos);
					hit.distance = Vector3f::Distance(origin, hit.point);
					hit.triangleIndex1 = intersection.triangleIndex1;
					hit.triangleIndex2 = intersection.triangleIndex2;
					hit.triangleIndex3 = intersection.triangleIndex3;
					hit.transform = intersection.transform;
					return true;
				}
			}
		}

		return false;
	}

    bool Physics::Raycast2(const Vector3 &origin, const Vector3 &direction, float maxDistance, RaycastHit &hit, Layer layerMask)
    {
        TriangleIntersection intersection;
        intersection.transform = nullptr;
	    intersection.triangleIndex1 = -1;
	    intersection.lastPos = std::numeric_limits<float>::max();
        float lastPos = std::numeric_limits<float>::max();
        uint32_t ignoreRaycast = static_cast<uint32_t>(Layer_IgnoreRaycast);

        Renderer *renderer = nullptr;
        size_t currentIndex = 0;

        while((renderer = Graphics::GetRendererByIndex(currentIndex++)) != nullptr)
        {
            if(!renderer->GetGameObject()->GetIsActive())
                continue;

            uint32_t layer = static_cast<uint32_t>(renderer->GetGameObject()->GetLayer());

            if(layer > 0)
            {
                if(layer & layerMask || layer & ignoreRaycast)
                    continue;
            }

            Transform *transform = renderer->GetTransform();
            Matrix4 transformation = transform->GetModelMatrix();
            Ray ray(transform->WorldToLocal(origin), transform->WorldToLocalVector(direction), FloatMaxValue);

            Mesh *mesh = nullptr;
            size_t meshIndex = 0;
            
            while((mesh = renderer->GetMesh(meshIndex++)) != nullptr)
            {
                auto &indices = mesh->GetIndices();
                
                if(indices.size() == 0)
                    continue;

                auto bounds = mesh->GetBounds();

                float distance = 0;
                
                if(!bounds.Intersects(ray, distance))
                    continue;

                auto &vertices = mesh->GetVertices();

                for(size_t j = 0; j < indices.size() / 3; j++)
                {
                    float currIntersectionPos = 0.0f;

                    Vector3 v1 = vertices[indices[j * 3]].position;
                    Vector3 v2 = vertices[indices[j * 3 + 1]].position;
                    Vector3 v3 = vertices[indices[j * 3 + 2]].position;

                    Vector4 v1t = transformation * Vector4(v1.x, v1.y, v1.z, 1.0f);
                    Vector4 v2t = transformation * Vector4(v2.x, v2.y, v2.z, 1.0f);
                    Vector4 v3t = transformation * Vector4(v3.x, v3.y, v3.z, 1.0f);

                    v1 = Vector3(v1t.x, v1t.y, v1t.z);
                    v2 = Vector3(v2t.x, v2t.y, v2t.z);
                    v3 = Vector3(v3t.x, v3t.y, v3t.z);

                    if (RayIntersectsTriangle(origin, direction, v1, v2, v3, currIntersectionPos))
                    {
                        if (currIntersectionPos < intersection.lastPos)
                        {
                            intersection.lastPos = currIntersectionPos;
                            intersection.triangleIndex1 = indices[j*3];
                            intersection.triangleIndex2 = indices[j*3+1];
                            intersection.triangleIndex3 = indices[j*3+2];
                            intersection.transform = renderer->GetTransform();
                            hit.normal = SurfaceNormalFromIndices(v1, v2, v3);
                        }
                    }
                }
            }
        }

        if(intersection.triangleIndex1 >= 0)
        {
            float totalDistance = Vector3f::Distance(origin, origin + (direction * intersection.lastPos));

            if(totalDistance <= maxDistance)
            {
                hit.point = origin + (direction * intersection.lastPos);
                hit.distance = Vector3f::Distance(origin, hit.point);
                hit.triangleIndex1 = intersection.triangleIndex1;
                hit.triangleIndex2 = intersection.triangleIndex2;
                hit.triangleIndex3 = intersection.triangleIndex3;
                hit.transform = intersection.transform;
                return true;
            }
        }

        return false;
    }

    bool Physics::BoxTest(const Vector3 &origin, const Vector3 &direction, float maxDistance, RaycastHit &hit, Layer layerMask)
    {
        hit.userData = nullptr;
        TriangleIntersection intersection;
        intersection.transform = nullptr;
	    intersection.triangleIndex1 = -1;
	    intersection.lastPos = std::numeric_limits<float>::max();
        float lastPos = std::numeric_limits<float>::max();
        uint32_t ignoreRaycast = static_cast<uint32_t>(Layer_IgnoreRaycast);

        Renderer *renderer = nullptr;
        size_t currentIndex = 0;

        while((renderer = Graphics::GetRendererByIndex(currentIndex++)) != nullptr)
        {
            if(!renderer->GetGameObject()->GetIsActive())
                continue;

            uint32_t layer = static_cast<uint32_t>(renderer->GetGameObject()->GetLayer());

            if(layer > 0)
            {
                if(layer & layerMask || layer & ignoreRaycast)
                    continue;
            }

            Transform *transform = renderer->GetTransform();
            Ray ray(transform->WorldToLocal(origin), transform->WorldToLocalVector(direction), FloatMaxValue);

            Mesh *mesh = nullptr;
            size_t meshIndex = 0;
            
            while((mesh = renderer->GetMesh(meshIndex++)) != nullptr)
            {
                auto &indices = mesh->GetIndices();
                
                if(indices.size() == 0)
                    continue;

                auto bounds = mesh->GetBounds();

                float currIntersectionPos = 0.0f;
                
                if(bounds.Intersects(ray, currIntersectionPos))
                {
                    if (currIntersectionPos < intersection.lastPos)
                    {
                        intersection.lastPos = currIntersectionPos;
                        intersection.userData = mesh;
                        intersection.transform = transform;
						intersection.userData = renderer;
                    }
                }
            }
        }

        if(intersection.userData != nullptr)
        {
            float totalDistance = Vector3f::Distance(origin, origin + (direction * intersection.lastPos));

            if(totalDistance <= maxDistance)
            {
                hit.userData = intersection.userData;
                hit.point = origin + (direction * intersection.lastPos);
                hit.distance = Vector3f::Distance(origin, hit.point);
                hit.triangleIndex1 = intersection.triangleIndex1;
                hit.triangleIndex2 = intersection.triangleIndex2;
                hit.triangleIndex3 = intersection.triangleIndex3;
                hit.transform = intersection.transform;
                return true;
            }
        }

        return false;
    }

    bool Physics::LineIntersects(const Vector3 &l1p1, const Vector3 &l1p2, const Vector3 &l2p1, const Vector3 &l2p2, Vector3 &hitpoint)
    {
        float x1 = l1p1.x;
        float y1 = l1p1.z;
        float x2 = l1p2.x;
        float y2 = l1p2.z;

        float x3 = l2p1.x;
        float y3 = l2p1.z;
        float x4 = l2p2.x;
        float y4 = l2p2.z;

        float denominator = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);

        if (glm::abs(denominator) <= FloatMinValue)
            return false;

        float t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / denominator;
        float u = -((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)) / denominator;

        if (t > 0 && t < 1 && u > 0)
        {
            hitpoint.x = x1 + t * (x2 - x1);
            hitpoint.z = y1 + t * (y2 - y1);
            return true;
        }
        else
            return false;
    }

    bool Physics::RayIntersectsTriangle(const Vector3 &origin, const Vector3 &dir, const Vector3 &v0, const Vector3 &v1, const Vector3 &v2, float &intersection)
    {
        intersection = 0;

        // Triangle edges
        Vector3 e1 = (v1 -v0);
        Vector3 e2 = (v2 -v0);

        const float epsilon = 0.000001f;

        Vector3 P, Q;
        //float i;
        float t;

        // Calculate determinant
        P = Vector3f::Cross(dir, e2);
        float det = Vector3f::Dot(e1, P);
        // If determinant is (close to) zero, the ray lies in the plane of the triangle or parallel it's plane
        if ((det > -epsilon) && (det < epsilon))
        {
            return false;
        }
        float invDet = 1.0f / det;

        // Distance from first vertex to ray origin
        Vector3 T = origin - v0;

        // Calculate u parameter
        float u = Vector3f::Dot(T, P) * invDet;
        // Intersection point lies outside of the triangle
        if ((u < 0.0f) || (u > 1.0f))
        {
            return false;
        }

        //Prepare to test v parameter
        Q = Vector3f::Cross(T, e1);

        // Calculate v parameter
        float v = Vector3f::Dot(dir, Q) * invDet;
        // Intersection point lies outside of the triangle
        if (v < 0.0f || u + v > 1.0f) 
            return false;

        // Calculate t
        t = Vector3f::Dot(e2, Q) * invDet;

        if (t > epsilon)
        {
            // Triangle interesected
            intersection = t;
            return true;
        }

        // No intersection
        return false;
    }

    Vector3 Physics::SurfaceNormalFromIndices(const Vector3 &pA, const Vector3 &pB, const Vector3 &pC)
    {
        Vector3 sideAB = pB - pA;
        Vector3 sideAC = pC - pA;
        return Vector3f::Normalize(Vector3f::Cross(sideAB, sideAC));
    }
}