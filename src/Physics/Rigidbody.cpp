#include "Rigidbody.hpp"
#include "Physics.hpp"
#include "Collider.hpp"
#include "BoxCollider.hpp"
#include "CapsuleCollider.hpp"
#include "CylinderCollider.hpp"
#include "TerrainCollider.hpp"
#include "MeshCollider.hpp"
#include "SphereCollider.hpp"
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
		static JPH::Shape *Create(Collider *collider);
	};

	Rigidbody::Rigidbody()
	{
		body = new PhysicsBody();
		body->handle = nullptr;
		body->interface = Physics::GetBodyInterface();
		mass = 1.0f;
	}

	Rigidbody::Rigidbody(float mass)
	{
		body = new PhysicsBody();
		body->handle = nullptr;
		body->interface = Physics::GetBodyInterface();
		this->mass = mass;
	}

	void Rigidbody::OnInitialize()
	{
		Initialize();
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

		delete body;
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

	}

	void Rigidbody::OnDeactivate()
	{

	}

	JPH::Body *Rigidbody::GetBody()
	{
		return body->handle;
	}

	bool Rigidbody::CreateShape()
	{
		auto colliders = GetGameObject()->GetComponentsOfType<Collider>();

		if(colliders.size() == 0)
		{
			return false;
		}

		if(colliders.size() == 1)
		{
			Collider *c = colliders[0];

			switch(c->GetType())
			{
				case ColliderType::Box:
				{
					BoxCollider *collider = static_cast<BoxCollider*>(c);
					Vector3 h = collider->GetSize() * 0.5f;
					JPH::Vec3 halfExtent(h.x, h.y, h.z);
					JPH::BoxShapeSettings settings(halfExtent);
					JPH::ShapeSettings::ShapeResult result = settings.Create();

					if(result.IsValid())
					{
						body->shape = result.Get();
						return true;
					}

					break;
				}
				case ColliderType::Capsule:
				{
					CylinderCollider *collider = static_cast<CylinderCollider*>(c);
					float halfHeight = collider->GetHeight() * 0.5f;
					float radius = collider->GetRadius();
					JPH::CapsuleShapeSettings settings(halfHeight, radius);
					JPH::ShapeSettings::ShapeResult result = settings.Create();

					if(result.IsValid())
					{
						body->shape = result.Get();
						return true;
					}

					break;
				}
				case ColliderType::Cylinder:
				{
					CylinderCollider *collider = static_cast<CylinderCollider*>(c);
					float halfHeight = collider->GetHeight() * 0.5f;
					float radius = collider->GetRadius();
					JPH::CylinderShapeSettings settings(halfHeight, radius);
					JPH::ShapeSettings::ShapeResult result = settings.Create();

					if(result.IsValid())
					{
						body->shape = result.Get();
						return true;
					}

					break;
				}
				case ColliderType::Mesh:
				{
					MeshCollider *collider = static_cast<MeshCollider*>(c);

					Mesh *mesh = collider->GetMesh();

					if(!mesh)
						return false;
					
					JPH::VertexList vertices;
					vertices.resize(mesh->GetVerticesCount());
					auto &mVertices = mesh->GetVertices();
					for(size_t i = 0; i < mVertices.size(); i++)
					{
						auto pos = mVertices[i].position;
						vertices[i] = JPH::Float3(pos.x, pos.y, pos.z);
					}

					JPH::IndexedTriangleList indices;
					indices.resize(mesh->GetIndicesCount());
					auto &mIndices = mesh->GetIndices();
					size_t index = 0;
					for(size_t i = 0; i < mIndices.size(); i+=3)
					{
						uint32_t i1 = mIndices[i+0];
						uint32_t i2 = mIndices[i+1];
						uint32_t i3 = mIndices[i+2];
						indices[index++] = JPH::IndexedTriangle(i1, i2, i3);
					}

					JPH::MeshShapeSettings settings(vertices, indices);
					JPH::ShapeSettings::ShapeResult result = settings.Create();

					if(result.IsValid())
					{
						body->shape = result.Get();
						return true;
					}

					break;
				}
				case ColliderType::Sphere:
				{
					SphereCollider *collider = static_cast<SphereCollider*>(c);
					JPH::SphereShapeSettings settings(collider->GetRadius());
					JPH::ShapeSettings::ShapeResult result = settings.Create();

					if(result.IsValid())
					{
						body->shape = result.Get();
						return true;
					}

					break;
				}
				case ColliderType::Terrain:
				{
					Terrain *terrain = GetGameObject()->GetComponent<Terrain>();
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
						body->shape = result.Get();
						return true;
					}

					break;
				}
			}
		}
		else
		{
			//Compound shape
			JPH::StaticCompoundShapeSettings settings;
			//settings.AddShape()
		}

		return false;
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
		if(Vector3f::IsNan(force))
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
		if(Vector3f::IsNan(torque))
			return;

		JPH::Vec3 t(torque.x, torque.y, torque.z);
		body->interface->AddTorque(body->id, t);
	}

	void Rigidbody::AddRelativeTorque(const Vector3 &torque)
	{
		if(Vector3f::IsNan(torque))
			return;

		float length = torque.length();
		Vector3 v = GetTransform()->InverseTransformDirection(torque) * length;
		JPH::Vec3 t(v.x, v.y, v.z);
		body->interface->AddTorque(body->id, t);
	}

	void Rigidbody::SetLinearVelocity(const Vector3 &velocity)
	{
		if(Vector3f::IsNan(velocity))
			return;

		JPH::Vec3 v(velocity.x, velocity.y, velocity.z);
		body->interface->SetLinearVelocity(body->id, v);
	}

	void Rigidbody::SetAngularVelocity(const Vector3 &velocity)
	{
		if(Vector3f::IsNan(velocity))
			return;

		JPH::Vec3 v(velocity.x, velocity.y, velocity.z);
		body->interface->SetAngularVelocity(body->id, v);
	}

	void Rigidbody::MovePosition(const Vector3 &position)
	{
		if(Vector3f::IsNan(position))
			return;

		JPH::Vec3 p(position.x, position.y, position.z);
		body->interface->SetPosition(body->id, p, JPH::EActivation::Activate);
	}

	void Rigidbody::MoveRotation(const Quaternion &rotation)
	{
		if(Quaternionf::IsNan(rotation))
			return;

		JPH::Quat r(rotation.x, rotation.y, rotation.z, rotation.w);
		body->interface->SetRotation(body->id, r, JPH::EActivation::Activate);
	}

	JPH::Shape *ShapeHelper::Create(Collider *collider)
	{
		switch(collider->GetType())
		{
			case ColliderType::Box:
			{
				BoxCollider *collider = static_cast<BoxCollider*>(collider);
				Vector3 h = collider->GetSize() * 0.5f;
				JPH::Vec3 halfExtent(h.x, h.y, h.z);
				JPH::BoxShapeSettings settings(halfExtent);
				JPH::ShapeSettings::ShapeResult result;

				auto shape = new JPH::BoxShape(settings, result);

				if(!result.IsValid())
				{
					delete shape;
					return nullptr;
				}

				return shape;
			}
			case ColliderType::Capsule:
			{
				CylinderCollider *collider = static_cast<CylinderCollider*>(collider);
				float halfHeight = collider->GetHeight() * 0.5f;
				float radius = collider->GetRadius();
				JPH::CapsuleShapeSettings settings(halfHeight, radius);
				JPH::ShapeSettings::ShapeResult result;
				auto shape = new JPH::CapsuleShape(settings, result);

				if(!result.IsValid())
				{
					delete shape;
					return nullptr;
				}

				return shape;
			}
			case ColliderType::Cylinder:
			{
				CylinderCollider *collider = static_cast<CylinderCollider*>(collider);
				float halfHeight = collider->GetHeight() * 0.5f;
				float radius = collider->GetRadius();
				JPH::CylinderShapeSettings settings(halfHeight, radius);
				JPH::ShapeSettings::ShapeResult result;
				auto shape = new JPH::CylinderShape(settings, result);

				if(!result.IsValid())
				{
					delete shape;
					return nullptr;
				}

				return shape;
			}
			case ColliderType::Mesh:
			{
				MeshCollider *collider = static_cast<MeshCollider*>(collider);

				Mesh *mesh = collider->GetMesh();

				if(!mesh)
					return nullptr;
				
				JPH::VertexList vertices;
				vertices.resize(mesh->GetVerticesCount());
				auto &mVertices = mesh->GetVertices();
				for(size_t i = 0; i < mVertices.size(); i++)
				{
					auto pos = mVertices[i].position;
					vertices[i] = JPH::Float3(pos.x, pos.y, pos.z);
				}

				JPH::IndexedTriangleList indices;
				indices.resize(mesh->GetIndicesCount());
				auto &mIndices = mesh->GetIndices();
				size_t index = 0;
				for(size_t i = 0; i < mIndices.size(); i+=3)
				{
					uint32_t i1 = mIndices[i+0];
					uint32_t i2 = mIndices[i+1];
					uint32_t i3 = mIndices[i+2];
					indices[index++] = JPH::IndexedTriangle(i1, i2, i3);
				}

				JPH::MeshShapeSettings settings(vertices, indices);
				JPH::ShapeSettings::ShapeResult result;
				auto shape = new JPH::MeshShape(settings, result);

				if(!result.IsValid())
				{
					delete shape;
					return nullptr;
				}

				return shape;
			}
			case ColliderType::Sphere:
			{
				SphereCollider *collider = static_cast<SphereCollider*>(collider);
				JPH::SphereShapeSettings settings(collider->GetRadius());
				JPH::ShapeSettings::ShapeResult result;
				auto shape = new JPH::SphereShape(settings, result);

				if(!result.IsValid())
				{
					delete shape;
					return nullptr;
				}

				return shape;
			}
			case ColliderType::Terrain:
			{
				Terrain *terrain = collider->GetGameObject()->GetComponent<Terrain>();
				if(!terrain)
				{
					return nullptr;
				}

				// Get terrain dimensions
				uint32_t width = terrain->GetWidth();
				uint32_t depth = terrain->GetDepth();
				float s = terrain->GetScale();

				// Create a grid of height values
				std::vector<float> heightData;
				heightData.resize(width * depth);

				for (uint32_t y = 0; y < depth; ++y)
				{
					for (uint32_t x = 0; x < width; ++x)
					{
						// Get the height at this point
						float height = terrain->GetHeightAtPoint(x, y);
						uint32_t index = y * width + x;
						heightData[index] = height;
					}
				}

				JPH::Vec3 offset(0, 0, 0);
				JPH::Vec3 scale(s, s, -s);

				JPH::HeightFieldShapeSettings settings(heightData.data(), offset, scale, width);
				JPH::Shape::ShapeResult result;
				auto shape = new JPH::HeightFieldShape(settings, result);

				if(!result.IsValid())
				{
					delete shape;
					return nullptr;
				}

				return shape;
			}
			default:
				return nullptr;
		}
	}
}