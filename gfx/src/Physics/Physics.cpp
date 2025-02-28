#include "Physics.hpp"
#include "../Core/Debug.hpp"
#include "../Core/GameObject.hpp"
#include "../Core/Transform.hpp"
#include "../Core/Time.hpp"
#include "../System/Numerics/Vector3.hpp"
#include "../System/Numerics/Matrix4.hpp"
#include "../Graphics/Graphics.hpp"
#include "../Graphics/Mesh.hpp"
#include "../Graphics/Renderers/Renderer.hpp"
#include "../External/glm/glm.hpp"
#include "Rigidbody.hpp"

#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/IssueReporting.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/Collision/ShapeCast.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/CollisionCollectorImpl.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>

#include <cstdint>
#include <thread>
#include <vector>

namespace GFX
{
    namespace Layers
    {
        static constexpr JPH::ObjectLayer NON_MOVING = 0;
        static constexpr JPH::ObjectLayer MOVING = 1;
        static constexpr JPH::ObjectLayer NUM_LAYERS = 2;
    };

    namespace BroadPhaseLayers
    {
        static constexpr JPH::BroadPhaseLayer NON_MOVING(0);
        static constexpr JPH::BroadPhaseLayer MOVING(1);
        static constexpr JPH::uint NUM_LAYERS(2);
    };

    class ExcludeBodiesFilter : public JPH::BodyFilter 
    {
    public:
        bool ShouldCollide(const JPH::BodyID& body_id) const override 
        {
            return !HasIgnoreRaycast(body_id);
        }

    private:
        std::vector<JPH::BodyID> mExcludeBodies;
        bool HasIgnoreRaycast(const JPH::BodyID& body_id) const
        {
            JPH::uint64 userData = Physics::GetBodyInterface()->GetUserData(body_id);
            Rigidbody *rb = reinterpret_cast<Rigidbody*>(userData);
            if(rb)
                return rb->GetGameObject()->GetLayer() == Layer_IgnoreRaycast;
            return false;
        }
    };

    /// Class that determines if two object layers can collide
    class ObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter
    {
    public:
        bool ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const override
        {
            switch (inObject1)
            {
            case Layers::NON_MOVING:
                return inObject2 == Layers::MOVING; // Non moving only collides with moving
            case Layers::MOVING:
                return true; // Moving collides with everything
            default:
                JPH_ASSERT(false);
                return false;
            }
        }
    };

    // This defines a mapping between object and broadphase layers.
    class BPLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface
    {
    public:
        BPLayerInterfaceImpl()
        {
            // Create a mapping table from object to broad phase layer
            mObjectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
            mObjectToBroadPhase[Layers::MOVING] = BroadPhaseLayers::MOVING;
        }

        JPH::uint GetNumBroadPhaseLayers() const override
        {
            return BroadPhaseLayers::NUM_LAYERS;
        }

        JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override
        {
            JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
            return mObjectToBroadPhase[inLayer];
        }

    #if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
        const char * GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override
        {
            switch ((JPH::BroadPhaseLayer::Type)inLayer)
            {
            case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING:	return "NON_MOVING";
            case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::MOVING:		return "MOVING";
            default:													JPH_ASSERT(false); return "INVALID";
            }
        }
    #endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

    private:
        JPH::BroadPhaseLayer mObjectToBroadPhase[Layers::NUM_LAYERS];
    };

    /// Class that determines if an object layer can collide with a broadphase layer
    class ObjectVsBroadPhaseLayerFilterImpl : public JPH::ObjectVsBroadPhaseLayerFilter
    {
    public:
        bool ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const override
        {
            switch (inLayer1)
            {
            case Layers::NON_MOVING:
                return inLayer2 == BroadPhaseLayers::MOVING;
            case Layers::MOVING:
                return true;
            default:
                JPH_ASSERT(false);
                return false;
            }
        }
    };

    // An example contact listener
    class MyContactListener : public JPH::ContactListener
    {
    public:
        // See: ContactListener
        JPH::ValidateResult	OnContactValidate(const JPH::Body &inBody1, const JPH::Body &inBody2, JPH::RVec3Arg inBaseOffset, const JPH::CollideShapeResult &inCollisionResult) override
        {
            //std::cout << "Contact validate callback" << std::endl;

            // Allows you to ignore a contact before it is created (using layers to not make objects collide is cheaper!)
            return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
        }

        void OnContactAdded(const JPH::Body &inBody1, const JPH::Body &inBody2, const JPH::ContactManifold &inManifold, JPH::ContactSettings &ioSettings) override
        {
            //std::cout << "A contact was added" << std::endl;
        }

        void OnContactPersisted(const JPH::Body &inBody1, const JPH::Body &inBody2, const JPH::ContactManifold &inManifold, JPH::ContactSettings &ioSettings) override
        {
            //std::cout << "A contact was persisted" << std::endl;
        }

        void OnContactRemoved(const JPH::SubShapeIDPair &inSubShapePair) override
        {
            //std::cout << "A contact was removed" << std::endl;
        }
    };

    // An example activation listener
    class MyBodyActivationListener : public JPH::BodyActivationListener
    {
    public:
        void OnBodyActivated(const JPH::BodyID &inBodyID, JPH::uint64 inBodyUserData) override
        {
            //std::cout << "A body got activated " << reinterpret_cast<uint64_t>(&inBodyID) << std::endl;
        }

        void OnBodyDeactivated(const JPH::BodyID &inBodyID, JPH::uint64 inBodyUserData) override
        {
            //std::cout << "A body went to sleep " << reinterpret_cast<uint64_t>(&inBodyID) << std::endl;
        }
    };

    struct PhysicsManager
    {
        JPH::JobSystemThreadPool jobSystem = JPH::JobSystemThreadPool(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, std::thread::hardware_concurrency() - 1);
        JPH::PhysicsSystem physicsSystem;
        JPH::TempAllocatorMalloc allocator;
        BPLayerInterfaceImpl broadphaseLayer;
        ObjectVsBroadPhaseLayerFilterImpl objectBroadphaseFilter;
        ObjectLayerPairFilterImpl objectLayerFilter;
        MyBodyActivationListener bodyActivationListener;
        MyContactListener contactListener;
        ExcludeBodiesFilter excludeBodiesFilter;
        std::vector<Rigidbody*> bodies;
    };

    static bool JoltAssertFailed(const char *inExpression, const char *inMessage, const char *inFile, JPH::uint inLine)
    {
        printf("%s:%u | %s | %s \n", inFile, inLine, inExpression, inMessage);
        return true; //Trigger breakpoint
    }

    std::unique_ptr<PhysicsManager> Physics::physicsManager = nullptr;
    float Physics::fixedTimeStep = 1.0f / 60;

    JPH::BodyInterface *Physics::GetBodyInterface()
	{
        auto &interface = physicsManager->physicsSystem.GetBodyInterface();
        return &interface;
	}

    float Physics::GetFixedTimeStep()
    {
        return fixedTimeStep;
    }

    void Physics::Initialize()
    {
        JPH::RegisterDefaultAllocator();

        // Create a factory, this class is responsible for creating instances of classes based on their name or hash and is mainly used for deserialization of saved data.
        // It is not directly used in this example but still required.
        JPH::Factory::sInstance = new JPH::Factory();

        JPH::RegisterTypes();

        const JPH::uint maxBodies = 2048;
        const JPH::uint numBodyMutexes = 0;
        const JPH::uint maxBodyPairs = 2048;
        const JPH::uint maxContactConstraints = 2048;

        physicsManager = std::make_unique<PhysicsManager>();
	    physicsManager->physicsSystem.Init(maxBodies, numBodyMutexes, maxBodyPairs, maxContactConstraints, physicsManager->broadphaseLayer, physicsManager->objectBroadphaseFilter, physicsManager->objectLayerFilter);
        physicsManager->physicsSystem.SetBodyActivationListener(&physicsManager->bodyActivationListener);
	    physicsManager->physicsSystem.SetContactListener(&physicsManager->contactListener);

    #ifdef JPH_ENABLE_ASSERTS
        typedef bool(*AssertFunction)(const char *, const char *, const char *, JPH::uint);
        JPH::AssertFailed = (AssertFunction)JoltAssertFailed;
    #endif
    }

    void Physics::Deinitialize()
    {
        JPH::UnregisterTypes();
        delete JPH::Factory::sInstance;
        JPH::Factory::sInstance = nullptr;
    }

    static double currentTime = 0.0;
    static double lastPhysicsUpdateTime = 0.0;

    void Physics::NewFrame()
    {
        const int cCollisionSteps = 1;

        physicsManager->physicsSystem.Update(fixedTimeStep, cCollisionSteps, &physicsManager->allocator, &physicsManager->jobSystem);

        auto interface = GetBodyInterface();

        currentTime = Time::GetTimeAsDouble();

        float t = (currentTime - lastPhysicsUpdateTime) / Time::GetDeltaTime();

        for(size_t i = 0; i < physicsManager->bodies.size(); i++)
        {
            auto b = physicsManager->bodies[i]->GetBody();
            
            if(b == nullptr)
                continue;
            
            auto id = b->GetID();

            if(!interface->IsActive(id))
                continue;
            
            auto pos = interface->GetPosition(id);
            auto rot = interface->GetRotation(id);
            Vector3 position(pos.GetX(), pos.GetY(), pos.GetZ());
            Quaternion rotation(rot.GetW(), rot.GetX(), rot.GetY(), rot.GetZ());

            if(t < 1.0f)
            {
                rotation = Quaternionf::Slerp(physicsManager->bodies[i]->GetTransform()->GetRotation(), rotation, t);
                position = Vector3f::Lerp(physicsManager->bodies[i]->GetTransform()->GetPosition(), position, t);
            }

            physicsManager->bodies[i]->GetTransform()->SetPosition(position);
            physicsManager->bodies[i]->GetTransform()->SetRotation(rotation);
        }

        if (t >= 1.0f) 
        {
            lastPhysicsUpdateTime = currentTime; // Update the last physics update time
        }
    }

    void Physics::Add(Rigidbody *rb)
    {
        for(size_t i = 0; i < physicsManager->bodies.size(); i++)
        {
            if(physicsManager->bodies[i] == rb)
                return;
        }
        
        physicsManager->bodies.push_back(rb);

        printf("Added a rigidbody\n");
    }

    void Physics::Remove(Rigidbody *rb)
    {
        size_t index = 0;
        bool found = false;
        for(size_t i = 0; i < physicsManager->bodies.size(); i++)
        {
            if(physicsManager->bodies[i] == rb)
            {
                index = i;
                found = true;
                break;
            }
        }

        if(found)
            physicsManager->bodies.erase(physicsManager->bodies.begin() + index);
    }

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

    bool Physics::RayTest(const Vector3 &origin, const Vector3 &direction, float maxDistance, RaycastHit &hit)
    {
        if(!physicsManager)
            return false;

        auto &narrowPhase = physicsManager->physicsSystem.GetNarrowPhaseQuery();
        
        JPH::Vec3 from(origin.x, origin.y, origin.z);
        JPH::Vec3 to = from + (JPH::Vec3(direction.x, direction.y, direction.z) * maxDistance);
        
        JPH::RRayCast ray(from, (to - from));
        JPH::RayCastResult result;

        if(narrowPhase.CastRay(ray, result, {}, {}, physicsManager->excludeBodiesFilter))
        {
            auto &bodyInterface = physicsManager->physicsSystem.GetBodyInterface();
            JPH::uint64 userData = bodyInterface.GetUserData(result.mBodyID);
            Rigidbody *rb = reinterpret_cast<Rigidbody*>(userData);

            //JPH::Vec3 point = from + (JPH::Vec3(direction.x, direction.y, direction.z) * result.mFraction);
            JPH::Vec3 point = ray.mOrigin + ray.mDirection * result.mFraction;
            hit.point = Vector3(point.GetX(), point.GetY(), point.GetZ());
            hit.distance = Vector3f::Distance(origin, hit.point);

            if(rb != nullptr)
            {
                hit.transform = rb->GetTransform();
                JPH::Vec3 normal = rb->GetBody()->GetWorldSpaceSurfaceNormal(result.mSubShapeID2, point);
                hit.normal = Vector3(normal.GetX(), normal.GetY(), normal.GetZ());
            }
            else
            {
                hit.transform = nullptr;
            }

            return true;
        }

        return false;
    }

    bool Physics::SphereTest(const Vector3 &origin, const Vector3 &direction, float maxDistance, float radius, RaycastHit &hit)
    {
        if(!physicsManager)
            return false;
            
        JPH::Vec3 from = JPH::Vec3(origin.x, origin.y, origin.z);
        JPH::RMat44 inCenterOfMassStart = JPH::RMat44::sTranslation(from);
        JPH::Vec3 inDirection = JPH::Vec3(direction.x, direction.y, direction.z) * maxDistance;
		JPH::SphereShape sphere(radius);
		sphere.SetEmbedded();
		JPH::RShapeCast shapeCast(&sphere, JPH::Vec3(1, 1, 1), inCenterOfMassStart, inDirection);
		JPH::ShapeCastSettings settings;
		JPH::ClosestHitCollisionCollector<JPH::CastShapeCollector> collector;

        auto &narrowPhase = physicsManager->physicsSystem.GetNarrowPhaseQuery();

		narrowPhase.CastShape(shapeCast, settings, from, collector, {}, {}, physicsManager->excludeBodiesFilter);

        if(collector.HadHit())
        {
            hit.point = origin + direction * collector.mHit.mFraction;
            hit.distance = Vector3f::Distance(origin, hit.point);
            JPH::Vec3 point(hit.point.x, hit.point.y, hit.point.z);

            auto &bodyInterface = physicsManager->physicsSystem.GetBodyInterface();
            JPH::uint64 userData = bodyInterface.GetUserData(collector.mHit.mBodyID2);
            Rigidbody *rb = reinterpret_cast<Rigidbody*>(userData);

            if(rb != nullptr)
            {
                hit.transform = rb->GetTransform();
                JPH::Vec3 normal = rb->GetBody()->GetWorldSpaceSurfaceNormal(collector.mHit.mSubShapeID2, point);
                hit.normal = Vector3(normal.GetX(), normal.GetY(), normal.GetZ());
            }
            else
            {
                hit.transform = nullptr;
            }

            return true;
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