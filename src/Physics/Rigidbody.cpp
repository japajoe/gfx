#include "Rigidbody.hpp"
#include "Physics.hpp"
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>

namespace GFX
{
	struct PhysicsBody
	{
		JPH::Body *handle;
	};

	void Rigidbody::OnInitialize()
	{
		//Body *floor = body_interface.CreateBody(floor_settings);
	}

	void Rigidbody::OnDestroy()
	{

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
}