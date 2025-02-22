#include "Rigidbody.hpp"
#include "Physics.hpp"
#include "Collision/Collider.hpp"
#include "Collision/BoxCollider.hpp"
#include "Collision/CapsuleCollider.hpp"
#include "Collision/CylinderCollider.hpp"
#include "Collision/TerrainCollider.hpp"
#include "Collision/MeshCollider.hpp"
#include "Collision/SphereCollider.hpp"
#include "../Core/GameObject.hpp"
#include "../Graphics/Renderers/Terrain.hpp"
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/HeightFieldShape.h>
#include <Jolt/Physics/Collision/Shape/MeshShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>
#include <Jolt/Physics/Collision/Shape/StaticCompoundShape.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>
#include <Jolt/Physics/Constraints/Constraint.h>
#include <Jolt/Physics/Constraints/HingeConstraint.h>
#include <vector>

#include "../Core/Debug.hpp"

namespace GFX
{
	struct PhysicsBody
	{
		JPH::Body *handle;
		JPH::RefConst<JPH::Shape> shape;
		JPH::BodyInterface *interface;
		JPH::BodyID id;
	};

	class ShapeHelper
	{
	public:
		static bool Create(Collider *c, JPH::RefConst<JPH::Shape> &outShape);
	};

	Rigidbody::Rigidbody() : Component()
	{
		body = std::make_unique<PhysicsBody>();
		body->handle = nullptr;
		body->interface = Physics::GetBodyInterface();
		mass = 1.0f;
		isActive = true;
	}

	Rigidbody::Rigidbody(float mass) : Component()
	{
		body = std::make_unique<PhysicsBody>();
		body->handle = nullptr;
		body->interface = Physics::GetBodyInterface();
		this->mass = mass;
		isActive = true;
	}

	Rigidbody::~Rigidbody() = default;

	void Rigidbody::OnInitialize()
	{
		Initialize();
	}

	bool Rigidbody::IsInitialized() const
	{
		if(!body)
			return false;
		if(!body->handle)
			return false;
		if(!body->interface)
			return false;
		return true;
	}

	void Rigidbody::OnDestroy()
	{
		if(body->handle)
		{
			JPH::BodyID bodies[1];
			bodies[0] = body->handle->GetID();

			auto interface = Physics::GetBodyInterface();
			interface->RemoveBodies(bodies, 1);
			interface->DestroyBody(body->handle->GetID());

			Physics::Remove(this);
		}

		body->handle = nullptr;
		body->interface = nullptr;
		body.reset();
		body = nullptr;
	}

	bool Rigidbody::Initialize()
	{
		if(body->handle)
		{
			return false;
		}

		if(!CreateShape())
		{
			return false;
		}

		auto pos = GetTransform()->GetPosition();
		auto rot = GetTransform()->GetRotation();

		JPH::Vec3 position(pos.x, pos.y, pos.z);
		JPH::Quat rotation = JPH::Quat(rot.x, rot.y, rot.z, rot.w);
		JPH::EMotionType motionType = GetMass() > 0 ? JPH::EMotionType::Dynamic : JPH::EMotionType::Static;
		PhysicsLayer layer = GetMass() > 0 ? PhysicsLayer::Moving : PhysicsLayer::NonMoving;

		JPH::BodyCreationSettings settings(body->shape, position, rotation, motionType, static_cast<JPH::ObjectLayer>(layer));
		JPH::MassProperties msp;
		msp.ScaleToMass(mass); //actual mass in kg
		settings.mMassPropertiesOverride = msp;
		settings.mOverrideMassProperties = JPH::EOverrideMassProperties::CalculateInertia;
		settings.mLinearDamping = 0.1f;
		settings.mAngularDamping = 0.1f;
		settings.mRestitution = 0.35f;
		settings.mAllowSleeping = true;

		body->handle = body->interface->CreateBody(settings);

		if(body->handle == nullptr)
		{
			return false;
		}

		body->id = body->handle->GetID();
		body->handle->SetUserData(reinterpret_cast<JPH::uint64>(this));

		JPH::BodyID bodies[1];
		bodies[0] = body->id;
		auto state = body->interface->AddBodiesPrepare(bodies, 1);
		body->interface->AddBodiesFinalize(bodies, 1, state, JPH::EActivation::Activate);

		Physics::Add(this);

		return true;
	}

	void Rigidbody::OnActivate()
	{
		isActive = true;

		if(!IsInitialized())
			return;
		body->interface->ActivateBody(body->id);
	}

	void Rigidbody::OnDeactivate()
	{
		isActive = false;

		if(!IsInitialized())
			return;
		body->interface->DeactivateBody(body->id);
	}

	JPH::Body *Rigidbody::GetBody()
	{
		return body->handle;
	}

	float Rigidbody::GetMass() const
	{
		return mass;
	}

	void Rigidbody::SetMass(float value)
	{
		mass = value;
	}

	void Rigidbody::AddForce(const Vector3 &force, ForceMode mode)
	{
		if(!IsInitialized())
			return;

		if(!isActive)
			return;

		if(Vector3f::IsNan(force))
			return;

		JPH::Vec3 f(force.x, force.y, force.z);
		if(mode == ForceMode::Force)
			body->interface->AddForce(body->id, f);
		else
			body->interface->AddImpulse(body->id, f);
	}

	void Rigidbody::AddForceAtPoint(const Vector3 &force, const Vector3 &point, ForceMode mode)
	{
		if(!IsInitialized())
			return;

		if(!isActive)
			return;

		if(Vector3f::IsNan(force) || Vector3f::IsNan(point))
			return;

		JPH::Vec3 f(force.x, force.y, force.z);
		JPH::Vec3 p(point.x, point.y, point.z);
		if(mode == ForceMode::Force)
			body->interface->AddForce(body->id, f, p);
		else
			body->interface->AddImpulse(body->id, f, p);
	}

	void Rigidbody::AddRelativeForce(const Vector3 &force, ForceMode mode)
	{
		if(!IsInitialized())
			return;

		if(!isActive)
			return;

		if(Vector3f::IsNan(force))
			return;

		float length = force.length();
		Vector3 v = GetTransform()->InverseTransformDirection(force) * length;
		JPH::Vec3 f(v.x, v.y, v.z);
		if(mode == ForceMode::Force)
			body->interface->AddForce(body->id, f);
		else
			body->interface->AddImpulse(body->id, f);
	}

	void Rigidbody::AddTorque(const Vector3 &torque)
	{
		if(!IsInitialized())
			return;

		if(!isActive)
			return;

		if(Vector3f::IsNan(torque))
			return;

		JPH::Vec3 t(torque.x, torque.y, torque.z);
		body->interface->AddTorque(body->id, t);
	}

	void Rigidbody::AddRelativeTorque(const Vector3 &torque)
	{
		if(!IsInitialized())
			return;

		if(!isActive)
			return;

		if(Vector3f::IsNan(torque))
			return;

		float length = torque.length();
		Vector3 v = GetTransform()->InverseTransformDirection(torque) * length;
		JPH::Vec3 t(v.x, v.y, v.z);
		body->interface->AddTorque(body->id, t);
	}

	void Rigidbody::SetLinearVelocity(const Vector3 &velocity)
	{
		if(!IsInitialized())
			return;

		if(!isActive)
			return;

		if(Vector3f::IsNan(velocity))
			return;

		JPH::Vec3 v(velocity.x, velocity.y, velocity.z);
		body->interface->SetLinearVelocity(body->id, v);
	}

	Vector3 Rigidbody::GetLinearVelocity() const
	{
		if(!IsInitialized())
			return Vector3(0, 0, 0);

		auto v = body->interface->GetLinearVelocity(body->id);
		return Vector3(v.GetX(), v.GetY(), v.GetZ());
	}

	void Rigidbody::SetAngularVelocity(const Vector3 &velocity)
	{
		if(!IsInitialized())
			return;

		if(!isActive)
			return;

		if(Vector3f::IsNan(velocity))
			return;

		JPH::Vec3 v(velocity.x, velocity.y, velocity.z);
		body->interface->SetAngularVelocity(body->id, v);
	}

	void Rigidbody::MovePosition(const Vector3 &position)
	{
		if(!IsInitialized())
			return;

		if(!isActive)
			return;

		if(Vector3f::IsNan(position))
			return;

		JPH::Vec3 p(position.x, position.y, position.z);
		body->interface->SetPosition(body->id, p, JPH::EActivation::Activate);
	}

	void Rigidbody::MoveRotation(const Quaternion &rotation)
	{
		if(!IsInitialized())
			return;

		if(!isActive)
			return;

		if(Quaternionf::IsNan(rotation))
			return;

		JPH::Quat r(rotation.x, rotation.y, rotation.z, rotation.w);
		body->interface->SetRotation(body->id, r, JPH::EActivation::Activate);
	}

	Vector3 Rigidbody::GetPointVelocity(const Vector3 &relativePosition)
	{
		if(!IsInitialized())
			return Vector3(0, 0, 0);

		if(Vector3f::IsNan(relativePosition))
			return Vector3(0, 0, 0);

		auto result = body->handle->GetPointVelocity(JPH::Vec3(relativePosition.x, relativePosition.y, relativePosition.z));
		return Vector3(result.GetX(), result.GetY(), result.GetZ());
		
		// JPH::RVec3 com = body->interface->GetCenterOfMassPosition(body->id);
        // JPH::RVec3 relPos(relativePosition.x - com.GetX(), relativePosition.y - com.GetY(), relativePosition.z - com.GetZ());

		// auto getPushVelocityInLocalPoint = [&] (const JPH::Vec3& rel_pos) -> JPH::Vec3 	{
		// 	JPH::RVec3 linearVelocity = body->interface->GetLinearVelocity(body->id);
		// 	JPH::RVec3 angularVelocity = body->interface->GetAngularVelocity(body->id);
		// 	// Compute the velocity at the given local position
		// 	return linearVelocity + angularVelocity.Cross(rel_pos);
		// };

        // JPH::RVec3 result = getPushVelocityInLocalPoint(relPos);
        // return Vector3(result.GetX(), result.GetY(), result.GetZ());
	}

	Vector3 Rigidbody::GetCenterOfMass()
	{
		if(!IsInitialized())
			return Vector3(0, 0, 0);

		JPH::RVec3 com = body->interface->GetCenterOfMassPosition(body->id);
		return Vector3(com.GetX(), com.GetY(), com.GetZ());
	}

	void Rigidbody::SetBounciness(float bounciness)
	{
		if(!IsInitialized())
			return;
		body->handle->SetRestitution(bounciness);
	}

	float Rigidbody::GetBounciness() const
	{
		if(!IsInitialized())
			return 0.0f;
		return body->interface->GetRestitution(body->id);
	}

	void Rigidbody::SetLinearDrag(float drag)
	{
		if(!IsInitialized())
			return;
		body->handle->GetMotionProperties()->SetLinearDamping(drag);
	}

	float Rigidbody::GetLinearDrag() const
	{
		if(!IsInitialized())
			return 0.0f;
		return body->handle->GetMotionProperties()->GetLinearDamping();
	}

	void Rigidbody::SetAngularDrag(float drag)
	{
		if(!IsInitialized())
			return;
		body->handle->GetMotionProperties()->SetAngularDamping(drag);
	}

	float Rigidbody::GetAngularDrag() const
	{
		if(!IsInitialized())
			return 0.0f;
		return body->handle->GetMotionProperties()->GetAngularDamping();
	}

	void Rigidbody::SetCollisionDetectionMode(CollisionDetectionMode mode)
	{
		if(!IsInitialized())
			return;
		body->interface->SetMotionQuality(body->id, static_cast<JPH::EMotionQuality>(mode));
	}

	CollisionDetectionMode Rigidbody::GetCollisionDetectionMode() const
	{
		if(!IsInitialized())
			return CollisionDetectionMode::Discrete;

		auto quality = body->interface->GetMotionQuality(body->id);
		return static_cast<CollisionDetectionMode>(quality);
	}

	
	void Rigidbody::SetGravityFactor(float factor)
	{
		if(!IsInitialized())
			return;
		body->interface->SetGravityFactor(body->id, factor);
	}

	float Rigidbody::GetGravityFactor() const
	{
		if(!IsInitialized())
			return 1.0f;
		return body->interface->GetGravityFactor(body->id);
	}

	bool Rigidbody::IsSleeping() const
	{
		if(!IsInitialized())
			return false;
		return body->interface->IsActive(body->id) == false;
	}

	void Rigidbody::SetConstraints(RigidbodyConstraints constraints)
	{
		if(!IsInitialized())
			return;
	}

	RigidbodyConstraints Rigidbody::GetConstraints() const
	{
		if(!IsInitialized())
			return RigidbodyConstraints::None;
		return static_cast<RigidbodyConstraints>(body->handle->GetMotionProperties()->GetAllowedDOFs());
	}

	bool Rigidbody::CreateShape()
	{
		auto colliders = GetGameObject()->GetComponentsOfTypeInChildren<Collider>();
		//auto colliders = GetGameObject()->GetComponentsOfType<Collider>();

		if(colliders.size() == 0)
		{
			return false;
		}

		if(colliders.size() == 1)
		{
			Collider *c = colliders[0];

			if(!ShapeHelper::Create(c, body->shape))
				return false;
			return true;
		}
		else
		{
			JPH::StaticCompoundShapeSettings settings;

			for(size_t i = 0; i < colliders.size(); i++)
			{
				Collider *c = colliders[i];

				JPH::RefConst<JPH::Shape> shape;

				if(!ShapeHelper::Create(c, shape))
				{
					return false;
				}
				
				Vector3 position = c->GetCenter() + c->GetTransform()->GetLocalPosition();
				Quaternion rotation = Quaternionf::Identity();
				settings.AddShape(JPH::Vec3(position.x, position.y, position.z), 
									JPH::Quat(rotation.x, rotation.y, rotation.z, rotation.w),
									shape, 0);
			}

			JPH::Shape::ShapeResult result = settings.Create();

			if(result.IsValid())
			{
				body->shape = result.Get();
				return true;
			}

			return false;
		}

		return false;
	}

	bool ShapeHelper::Create(Collider *c, JPH::RefConst<JPH::Shape> &outShape)
	{
		switch(c->GetType())
		{
			case ColliderType::Box:
			{
				BoxCollider *collider = static_cast<BoxCollider*>(c);
				Vector3 h = collider->GetSize() * 0.5f;
				Vector3 c = collider->GetCenter();
				JPH::Vec3 halfExtent(h.x, h.y, h.z);
				JPH::RVec3 center(c.x, c.y, c.z);
				JPH::BoxShapeSettings settings(halfExtent);
				JPH::ShapeSettings::ShapeResult result = settings.Create();

				if(result.IsValid())
				{
					JPH::RotatedTranslatedShapeSettings offsetShape(center, JPH::Quat::sIdentity(), result.Get());

					auto r = offsetShape.Create();

					if(r.IsValid())
					{
						outShape = r.Get();
						return true;
					}
					
					return false;
				}

				break;
			}
			case ColliderType::Capsule:
			{
				CylinderCollider *collider = static_cast<CylinderCollider*>(c);
				float halfHeight = collider->GetHeight() * 0.5f;
				float radius = collider->GetRadius();
				Vector3 c = collider->GetCenter();
				JPH::RVec3 center(c.x, c.y, c.z);
				JPH::CapsuleShapeSettings settings(halfHeight, radius);
				JPH::ShapeSettings::ShapeResult result = settings.Create();

				if(result.IsValid())
				{
					JPH::RotatedTranslatedShapeSettings offsetShape(center, JPH::Quat::sIdentity(), result.Get());

					auto r = offsetShape.Create();

					if(r.IsValid())
					{
						outShape = r.Get();
						return true;
					}
					
					return false;
				}

				break;
			}
			case ColliderType::Cylinder:
			{
				CylinderCollider *collider = static_cast<CylinderCollider*>(c);
				float halfHeight = collider->GetHeight() * 0.5f;
				float radius = collider->GetRadius();
				Vector3 c = collider->GetCenter();
				JPH::RVec3 center(c.x, c.y, c.z);
				JPH::CylinderShapeSettings settings(halfHeight, radius);
				JPH::ShapeSettings::ShapeResult result = settings.Create();

				if(result.IsValid())
				{
					JPH::RotatedTranslatedShapeSettings offsetShape(center, JPH::Quat::sIdentity(), result.Get());

					auto r = offsetShape.Create();

					if(r.IsValid())
					{
						outShape = r.Get();
						return true;
					}
					
					return false;
				}

				break;
			}
			case ColliderType::Mesh:
			{
				MeshCollider *collider = static_cast<MeshCollider*>(c);

				Mesh *mesh = collider->GetMesh();

				if(!mesh)
					return false;
				
				auto &mVertices = mesh->GetVertices();
				JPH::VertexList vertices;
				vertices.resize(mVertices.size());

				for(size_t i = 0; i < mVertices.size(); i++)
				{
					auto pos = mVertices[i].position;
					vertices[i] = JPH::Float3(pos.x, pos.y, pos.z);
				}

				auto &mIndices = mesh->GetIndices();
				JPH::IndexedTriangleList indices;
				indices.resize(mIndices.size() / 3);
				size_t index = 0;

				for(size_t i = 0; i < mIndices.size(); i+=3)
				{
					uint32_t i1 = mIndices[i+0];
					uint32_t i2 = mIndices[i+1];
					uint32_t i3 = mIndices[i+2];
					indices[index++] = JPH::IndexedTriangle(i1, i2, i3);
				}

				JPH::MeshShapeSettings settings(std::move(vertices), std::move(indices));
				JPH::ShapeSettings::ShapeResult result = settings.Create();

				if(result.IsValid())
				{
					outShape = result.Get();
					return true;
				}

				break;
			}
			case ColliderType::Sphere:
			{
				SphereCollider *collider = static_cast<SphereCollider*>(c);
				Vector3 c = collider->GetCenter();
				JPH::RVec3 center(c.x, c.y, c.z);
				JPH::SphereShapeSettings settings(collider->GetRadius());
				JPH::ShapeSettings::ShapeResult result = settings.Create();

				if(result.IsValid())
				{
					JPH::RotatedTranslatedShapeSettings offsetShape(center, JPH::Quat::sIdentity(), result.Get());

					auto r = offsetShape.Create();

					if(r.IsValid())
					{
						outShape = r.Get();
						return true;
					}
					
					return false;
				}

				break;
			}
			case ColliderType::Terrain:
			{
				Terrain *terrain = c->GetGameObject()->GetComponent<Terrain>();
				if(!terrain)
				{
					return false;
				}
				
				auto mesh = terrain->GetMesh(0);

				// Get terrain dimensions
				uint32_t width = terrain->GetWidth() + 1;
				uint32_t depth = terrain->GetDepth() + 1;
				float s = terrain->GetScale();

				// Create a grid of height values
				std::vector<float> heightData;
				heightData.resize(width * depth);

				uint32_t index = 0;

				for (uint32_t y = 0; y < depth; y++)
				{
					for (uint32_t x = 0; x < width; x++)
					{
						// Get the height at this point
						float height = terrain->GetHeightAtPoint(x, depth - y);
						index = y * width + x;
						heightData[index] = height;
					}
				}

				JPH::Vec3 offset(0, 0, -1.0f * depth * s);

				JPH::Vec3 scale(s, 1.0f, s);

				JPH::HeightFieldShapeSettings settings(heightData.data(), offset, scale, width);
				//JPH::uint32 sBlockSizeShift = 2;
				//settings.mBlockSize = 1 << sBlockSizeShift;
				//settings.mBitsPerSample = 8;

				JPH::Shape::ShapeResult result = settings.Create();

				if(result.IsValid())
				{
					outShape = result.Get();
					return true;
				}

				break;
			}
		}

		return false;
	}
}