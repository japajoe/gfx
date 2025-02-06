#ifndef GFX_PHYSICS_HPP
#define GFX_PHYSICS_HPP

#include "../Graphics/Ray.hpp"
#include "../Core/GameObject.hpp"
#include "RaycastHit.hpp"

namespace GFX
{
    class Physics
    {
    public:
        static bool Raycast(const Ray &ray, RaycastHit &hit, Layer layerMask = Layer_None);
		static bool Raycast(const Vector3 &origin, const Vector3 &direction, float maxDistance, RaycastHit &hit, Layer layerMask = Layer_None);
        static bool Raycast2(const Vector3 &origin, const Vector3 &direction, float maxDistance, RaycastHit &hit, Layer layerMask = Layer_None);
        static bool BoxTest(const Vector3 &origin, const Vector3 &direction, float maxDistance, RaycastHit &hit, Layer layerMask = Layer_None);
    private:
        static bool LineIntersects(const Vector3 &l1p1, const Vector3 &l1p2, const Vector3 &l2p1, const Vector3 &l2p2, Vector3 &hitpoint);
        static bool RayIntersectsTriangle(const Vector3 &origin, const Vector3 &dir, const Vector3 &v0, const Vector3 &v1, const Vector3 &v2, float &intersection);
        static Vector3 SurfaceNormalFromIndices(const Vector3 &pA, const Vector3 &pB, const Vector3 &pC);
    };
}

#endif