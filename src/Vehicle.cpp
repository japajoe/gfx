#include "Vehicle.hpp"

void Vehicle::OnInitialize()
{
	rigidbody = nullptr;
}

void Vehicle::Configure(const VehicleSettings &settings)
{
	if(rigidbody)
		return;
		
	wheels.reserve(4);

	wheels.push_back(settings.frontLeft);
	wheels.push_back(settings.frontRight);
	wheels.push_back(settings.rearLeft);
	wheels.push_back(settings.rearRight);

	auto collider = GetGameObject()->AddComponent<BoxCollider>();
	collider->SetSize(settings.colliderSize);
	collider->SetCenter(settings.colliderPosition);

	rigidbody = GetGameObject()->AddComponent<Rigidbody>(settings.mass);

	wheelBase = Vector3f::Distance(settings.frontLeft.collider->GetWheelPosition(), settings.rearLeft.collider->GetWheelPosition());
	rearTrack = Vector3f::Distance(settings.rearLeft.collider->GetWheelPosition(), settings.rearRight.collider->GetWheelPosition());
	turnRadius = settings.turnRadius;

	for(size_t i = 0; i < wheels.size(); i++)
		wheels[i].collider->SetRigidbody(rigidbody);
}

void Vehicle::OnUpdate()
{
    if(!rigidbody)
        return;

    float vertical = Input::GetAxis("Vertical");
    float horizontal = Input::GetAxis("Horizontal");

    if(glm::abs(vertical) > 0.0f)
    {
        wheels[0].collider->SetMotorTorque(vertical * 1000 * -1.0f);
        wheels[1].collider->SetMotorTorque(vertical * 1000 * -1.0f);
    }
    else
    {
        wheels[0].collider->SetMotorTorque(0);
        wheels[1].collider->SetMotorTorque(0);
    }

    if(glm::abs(horizontal) > 0.0f)
    {
        float angleLeft, angleRight;

		if(horizontal > 0.0f) //Steering right
		{
			angleLeft = glm::degrees(glm::atan(wheelBase / (turnRadius + (rearTrack / 2.0f)))) * horizontal;
			angleRight = glm::degrees(glm::atan(wheelBase / (turnRadius - (rearTrack / 2.0f)))) * horizontal;
		}
		else //Steering left
		{
			angleLeft = glm::degrees(glm::atan(wheelBase / (turnRadius - (rearTrack / 2.0f)))) * horizontal;
			angleRight = glm::degrees(glm::atan(wheelBase / (turnRadius + (rearTrack / 2.0f)))) * horizontal;
		}

		wheels[0].collider->SetSteerAngle(angleLeft * -1.0f);
		wheels[1].collider->SetSteerAngle(angleRight * -1.0f);
    }
    else
    {
        wheels[0].collider->SetSteerAngle(0);
        wheels[1].collider->SetSteerAngle(0);
    }

    for(auto &wheel : wheels)
    {
        Vector3 position;
        Quaternion rotation;
        wheel.collider->GetWorldPose(position, rotation);
        wheel.visual->SetRotation(rotation);
    }
}