#include "Wheel.hpp"
#include "../Rigidbody.hpp"
#include "../Physics.hpp"
#include "../../Core/Transform.hpp"
#include "../../Core/GameObject.hpp"
#include "../../Core/Debug.hpp"
#include "../../External/glm/glm.hpp"

namespace GFX
{
	Wheel::Wheel() : GameBehaviour()
	{
		rigidBody = nullptr;
        radius = 0.5f;
        restLength = 1.0f;
        springTravel = 0.5f;
        minLength = 0.0f;
        maxLength = 0.0f;
		springDamper = 50.0f;
		springStrength = 100.0f;
	}

	void Wheel::OnInitialize()
	{
		transform = GetTransform();
		rigidBody = GetTransform()->GetRoot()->GetGameObject()->GetComponent<Rigidbody>();
	}

	void Wheel::OnFixedUpdate()
	{
		if(!rigidBody)
			return;

		SetMinMaxLength();

        Vector3 offset = GetWheelOffset();
        Vector3 start = transform->GetPosition() + offset;
        //Vector3 end = start + (-transform->GetUp() * (maxLength + radius));
		Vector3 end = -transform->GetUp();

        RaycastHit hit;

		if(Physics::RayTest(start, end, maxLength, hit))
		{
			//ApplySpringForce(hit.distance);
			ApplySpringForce(hit);
		}

		Debug::DrawLine(start, start + (end * maxLength), Color::Red());
	}

	void Wheel::ApplySpringForce(RaycastHit hit)
	{
		float mass = rigidBody->GetMass();
		float gravity = 9.81f;
		float force = mass * gravity * 0.3f;
		Vector3 direction = -Vector3f::Normalize(hit.point - GetWheelOffset());
		Vector3 forcePoint = GetWheelOffset();
		//rigidBody->AddForceAtPoint(direction * force, forcePoint, ForceMode::Force);
		rigidBody->AddForceAtPoint(transform->GetUp() * force, hit.point);
	}

	// void Wheel::ApplySpringForce(float rayDistance)
	// {
	// 	float suspensionCompression = restLength - rayDistance;  // How much the suspension has compressed
    //     suspensionCompression = glm::clamp(suspensionCompression, 0.0f, restLength); // Clamp it to prevent over-compression

    //     // Step 4: Apply the spring force (Hooke's Law)
    //     float springForce = springStrength * suspensionCompression;  // Spring force (Hooke's Law)
        
    //     // Step 5: Calculate the damping force (simple damping model based on velocity)
    //     Vector3 wheelVelocity = rigidBody->GetLinearVelocity();
    //     float dampingForce = springDamper * wheelVelocity.z;  // Apply damping along the Z-axis (assuming Z is up/down)

    //     // Step 6: Apply the suspension forces
    //     Vector3 suspensionForce(0.0f, springForce - dampingForce * -1.0f, 0);  // Force along the Z-axis

    //     // Apply the force to the wheel body
    //     //bodyInterface.ApplyForce(wheelBody, suspensionForce);

	// 	const float mass = rigidBody->GetMass();
	// 	const float gravity = 9.81f;
	// 	const float maxForce = mass * gravity;
	// 	float force = suspensionForce.y;
	// 	if(force < maxForce)
	// 		force = maxForce;

	// 	//rigidBody->AddForce(suspensionForce, ForceMode::Impulse);
	// 	rigidBody->AddForceAtPoint(transform->GetUp() * force, transform->GetPosition(), ForceMode::Force);
	// }

	void Wheel::ApplySpringForce(float rayDistance, float unused)
	{
		// World-space direction of the spring force.
		Vector3 springDir = transform->GetUp();

		// World-space velocity of this tire.
		Vector3 tireWorldVel = rigidBody->GetPointVelocity(transform->GetPosition());

		// Calculate the offset from the raycast.
		float offset = restLength - rayDistance;

		// Calculate velocity along the spring direction.
		// Note that springDir is a unit vector, so this returns the magnitude of tireWorldVel
		float vel = Vector3f::Dot(springDir, tireWorldVel);

		// Calculate the magnitude of the dampened spring force.
		float force = (offset * springStrength) - (vel * springDamper);

		Vector3 totalForce = force * springDir;

		Vector3 forcePoint = GetWheelOffset();

		//rigidBody->AddForceAtPoint(force * springDir, transform->GetPosition());
		rigidBody->AddForceAtPoint(totalForce, forcePoint);
	}

	void Wheel::SetCenter(const Vector3 &center)
	{
		this->center = center;
	}

	Vector3 Wheel::GetCenter() const
	{
		return center;
	}

    Vector3 Wheel::GetWheelOffset() const
    {
        Vector3 offset =    center.x * transform->GetRight() +
                            center.y * transform->GetUp() +
                            center.z * transform->GetForward();
        return offset;
    }

    void Wheel::SetMinMaxLength()
    {
        minLength = restLength - springTravel;
        maxLength = restLength + springTravel;
    }
}
