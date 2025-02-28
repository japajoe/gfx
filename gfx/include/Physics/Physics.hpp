#ifndef GFX_PHYSICS_HPP
#define GFX_PHYSICS_HPP

#include "../Core/GameObject.hpp"
#include "RaycastHit.hpp"
#include <memory>

namespace JPH
{
    class BodyInterface;
};

namespace GFX
{
    struct PhysicsManager;
    class Rigidbody;

    enum class PhysicsLayer
    {
        NonMoving = 0,
        Moving = 1,
        Count = 2
    };

    class Physics
    {
    friend class Application;
    friend class GameBehaviour;
    private:
        static std::unique_ptr<PhysicsManager> physicsManager;
        static float fixedTimeStep;
        static bool LineIntersects(const Vector3 &l1p1, const Vector3 &l1p2, const Vector3 &l2p1, const Vector3 &l2p2, Vector3 &hitpoint);
        static bool RayIntersectsTriangle(const Vector3 &origin, const Vector3 &dir, const Vector3 &v0, const Vector3 &v1, const Vector3 &v2, float &intersection);
        static Vector3 SurfaceNormalFromIndices(const Vector3 &pA, const Vector3 &pB, const Vector3 &pC);
        static void Initialize();
        static void Deinitialize();
        static void NewFrame();
    public:
        static JPH::BodyInterface *GetBodyInterface();
        static float GetFixedTimeStep();
        static bool Raycast(const Ray &ray, RaycastHit &hit, Layer layerMask = Layer_None);
		static bool Raycast(const Vector3 &origin, const Vector3 &direction, float maxDistance, RaycastHit &hit, Layer layerMask = Layer_None);
        static bool BoxTest(const Vector3 &origin, const Vector3 &direction, float maxDistance, RaycastHit &hit, Layer layerMask = Layer_None);
        static bool RayTest(const Vector3 &origin, const Vector3 &direction, float maxDistance, RaycastHit &hit);
        static bool SphereTest(const Vector3 &origin, const Vector3 &direction, float maxDistance, float radius, RaycastHit &hit);
        static void Add(Rigidbody *rb);
        static void Remove(Rigidbody *rb);
    };
}

#endif