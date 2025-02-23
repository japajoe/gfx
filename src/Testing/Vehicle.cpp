#include "Vehicle.hpp"

void AntiRollBar::Update()
{
    Vector3 hitpoint;
    float travelL = 1.0f;
    float travelR = 1.0f;

    bool groundedLeft = left->GetGroundHit(hitpoint);

    if(groundedLeft)
    {
        travelL = (-left->GetTransform()->InverseTransformPoint(hitpoint).y - left->GetWheelRadius()) / left->GetRestLength();
    }
    
    bool groundedRight = right->GetGroundHit(hitpoint);

    if(groundedRight)
    {
        travelR = (-right->GetTransform()->InverseTransformPoint(hitpoint).y - right->GetWheelRadius()) / right->GetRestLength();
    }

    float force = (travelL - travelR) * antiRoll;

    if(groundedLeft)
        body->AddForceAtPoint(left->GetTransform()->GetUp() * -force, left->GetTransform()->GetPosition());

    if(groundedRight)
        body->AddForceAtPoint(right->GetTransform()->GetUp() * force, right->GetTransform()->GetPosition());
}

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

    antiRollBar.left = settings.frontLeft.collider;
    antiRollBar.right = settings.frontRight.collider;
    antiRollBar.body = rigidbody;
    antiRollBar.antiRoll = settings.antiRoll;

    auto audiosource = GetGameObject()->AddComponent<AudioSource>();
    audiosource->SetSpatial(true);
    audiosource->SetDopplerFactor(0.1f);
    audiosource->SetMinDistance(10.0f);
    audiosource->SetMaxDistance(1000.0f);
    auto fmGenerator = audiosource->AddGenerator<FMGenerator>(WaveType::Sine, 110.0f, 1.0f);
    fmGenerator->AddOperator(WaveType::Sine, 55.0f, 1.0f);
    fmGenerator->AddOperator(WaveType::Sine, 22.0f, 0.5f);
    audiosource->Play();
    oscillator = fmGenerator->GetCarrier();
}

void Vehicle::OnUpdate()
{
    if(!rigidbody)
        return;

    float vertical = Input::GetAxis("Vertical");
    float horizontal = Input::GetAxis("Horizontal");

    if(vertical > 0.0f)
    {
        wheels[0].collider->SetMotorTorque(vertical * 1000 * -1.0f);
        wheels[1].collider->SetMotorTorque(vertical * 1000 * -1.0f);
    }
    else if(vertical < 0.0f)
    {
        wheels[0].collider->SetBrakeTorque(vertical * 2000 * 1.0f);
        wheels[1].collider->SetBrakeTorque(vertical * 2000 * 1.0f);
    }
    else
    {
        wheels[0].collider->SetMotorTorque(0);
        wheels[1].collider->SetMotorTorque(0);
        wheels[0].collider->SetBrakeTorque(0);
        wheels[1].collider->SetBrakeTorque(0);
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

    float speed = GetSpeed();
    float frequency = 55.0f + Mathf::InverseLerp(0.0f, 200.0f, speed) * 220.0f;
    oscillator->SetFrequency(frequency);
}

void Vehicle::OnFixedUpdate()
{
    if(!rigidbody)
        return;
    antiRollBar.Update();
}

void Vehicle::Flip()
{
    if(!rigidbody)
        return;

    rigidbody->SetLinearVelocity(Vector3f::Zero());
    rigidbody->SetAngularVelocity(Vector3f::Zero());
    rigidbody->MoveRotation(Quaternionf::Identity());
}

float Vehicle::GetSpeed() const
{
    if(!rigidbody)
        return 0.0f;
    return glm::length(rigidbody->GetLinearVelocity()) * 3.6f;
}