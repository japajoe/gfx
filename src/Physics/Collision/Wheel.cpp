#include "Wheel.hpp"
#include "../Rigidbody.hpp"
#include "../Physics.hpp"
#include "../../Core/Transform.hpp"
#include "../../Core/GameObject.hpp"
#include "../../Core/Debug.hpp"
#include "../../Core/Time.hpp"
#include "../../External/glm/glm.hpp"

namespace GFX
{
	Wheel::Wheel() : GameBehaviour()
	{
		rigidBody = nullptr;
        radius = 0.5f;
        restLength = 1.2f;
        springTravel = 0.25f;
        minLength = 0.0f;
        maxLength = 0.0f;
		springDamper = 2000.0f;
		springStrength = 8000.0f;
		isEnabled = true;
		isGrounded = false;
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
		Vector3 end = -transform->GetUp();

        RaycastHit hit;

		if(Physics::RayTest(start, end, maxLength, hit))
		{
			if(hit.transform->GetRoot() != transform->GetRoot())
			{
				isGrounded = true;
				ApplySpringForce(hit);
			}
			else
			{
				isGrounded = false;
			}
		}
		else
		{
			isGrounded = false;
		}

		Debug::DrawLine(start, start + (end * maxLength), Color::Red());
	}

	void Wheel::ApplySpringForce(RaycastHit hit)
	{
		Vector3 springDir = transform->GetUp();
		Vector3 tireWorldVel = rigidBody->GetPointVelocity(transform->GetPosition());
		float offset = restLength - hit.distance;
		float vel = Vector3f::Dot(springDir, tireWorldVel);
		float force = (offset * springStrength) - (vel * springDamper);
		Vector3 totalForce = force * springDir;
		rigidBody->AddForceAtPoint(totalForce, hit.point);
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

	void Wheel::SetRestLength(float length)
	{
		this->restLength = length;
	}

	float Wheel::GetRestLength() const
	{
		return restLength;
	}

	void Wheel::SetSpringDamper(float value)
	{
		this->springDamper = value;
	}

	float Wheel::GetSpringDamper() const
	{
		return springDamper;
	}

	void Wheel::SetSpringStrength(float value)
	{
		this->springStrength = value;
	}

	float Wheel::GetSpringStrength() const
	{
		return springStrength;
	}

    void Wheel::SetIsEnabled(bool isEnabled)
    {
        this->isEnabled = isEnabled;    
    }

    bool Wheel::GetIsEnabled() const
    {
        return isEnabled;
    }

    bool Wheel::GetIsGrounded() const
    {
        return isGrounded;
    }
}
