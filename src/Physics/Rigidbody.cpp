#include "Rigidbody.hpp"
#include "Physics.hpp"
#include "Collider.hpp"
#include "BoxCollider.hpp"
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
#include <vector>

namespace GFX
{
	struct PhysicsBody
	{
		JPH::Body *handle;
		//JPH::Ref<JPH::Shape> shape;
		JPH::RefConst<JPH::Shape> shape;
	};

	Rigidbody::Rigidbody()
	{
		//body = std::make_unique<PhysicsBody>();
		body = new PhysicsBody();
		body->handle = nullptr;
		mass = 1.0f;
	}

	Rigidbody::Rigidbody(float mass)
	{
		body = new PhysicsBody();
		body->handle = nullptr;
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

		JPH::RVec3 position(pos.x, pos.y, pos.z);
		JPH::Quat rotation = JPH::Quat(rot.x, rot.y, rot.z, rot.w);
		JPH::EMotionType motionType = GetMass() > 0 ? JPH::EMotionType::Dynamic : JPH::EMotionType::Static;
		PhysicsLayer layer = GetMass() > 0 ? PhysicsLayer::Moving : PhysicsLayer::NonMoving;

		JPH::BodyCreationSettings settings(body->shape, position, rotation, motionType, static_cast<JPH::ObjectLayer>(layer));
		settings.mMassPropertiesOverride.mMass = mass;
		settings.mMassPropertiesOverride.mInertia = body->shape->GetMassProperties().mInertia;

		auto interface = Physics::GetBodyInterface();
		body->handle = interface->CreateBody(settings);

		if(body->handle == nullptr)
		{
			return false;
		}

		JPH::BodyID bodies[1];
		bodies[0] = body->handle->GetID();
		auto state = interface->AddBodiesPrepare(bodies, 1);
		interface->AddBodiesFinalize(bodies, 1, state, JPH::EActivation::Activate);

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
					JPH::Vec3Arg halfExtent(h.x, h.y, h.z);
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
					break;
				}
				case ColliderType::Cylinder:
				{
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

					JPH::Vec3Arg offset(0, 0, 0);
					JPH::Vec3Arg scale(s, 1.0f, -s);

					JPH::HeightFieldShapeSettings settings(heightData.data(), offset, scale, width);
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
}