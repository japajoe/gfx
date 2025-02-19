#include "Car.hpp"

void Car::OnInitialize()
{
	angleLeft = 0.0f;
	angleRight = 0.0f;

	auto cubeObject = GameObject::CreatePrimitive(PrimitiveType::Cube);
	cubeObject->GetTransform()->SetScale(Vector3(1.8f, 1.6f, 4.5f));
	cubeObject->GetTransform()->SetParent(GetTransform());

	auto transform = GetTransform();

	auto collider = GetGameObject()->AddComponent<BoxCollider>();
	collider->SetSize(Vector3(1.8f, 1.6f, 4.5f));

	rigidbody = GetGameObject()->AddComponent<Rigidbody>(1000.0f);

	auto createWheel = [] (const Vector3 &position, Transform *parent) -> WheelCollider* {
		auto w = GameObject::Create();
		w->GetTransform()->SetParent(parent);
		w->GetTransform()->SetPosition(position);
		auto col = w->AddComponent<WheelCollider>();
		col->springStiffness = 20000.0f;
		col->damperStiffness = 2500.0f;
		col->restLength = 0.1f;
		col->center = Vector3(0, -0.4f, 0);
		return col;
	};

	auto frontLeft = createWheel(Vector3(-2, 0, -2.25f), transform);
	auto frontRight = createWheel(Vector3(2, 0, -2.25f), transform);
	auto rearLeft = createWheel(Vector3(-2, 0, 2.25f), transform);
	auto rearRight = createWheel(Vector3(2, 0, 2.25f), transform);

	wheels.reserve(4);
	wheels.push_back(frontLeft);
	wheels.push_back(frontRight);
	wheels.push_back(rearLeft);
	wheels.push_back(rearRight);

	rigidbody->MovePosition(Vector3(0, 10, 0));

	auto orbit = Camera::GetMain()->GetGameObject()->AddComponent<MouseOrbit>();
	orbit->SetTarget(transform);

	wheelBase = Vector3f::Distance(frontLeft->GetWheelPosition(), rearLeft->GetWheelPosition());
	rearTrack = Vector3f::Distance(rearLeft->GetWheelPosition(), rearRight->GetWheelPosition());
	turnRadius = 10.8f;

}

void Car::OnUpdate()
{
	float vertical = Input::GetAxis("Vertical");
	float horizontal = Input::GetAxis("Horizontal");

	if(glm::abs(vertical) > 0)
	{
		wheels[0]->motorTorque = vertical * 1000.0f;
		wheels[1]->motorTorque = vertical * 1000.0f;
	}
	else
	{
		wheels[0]->motorTorque = 0.0f;
		wheels[1]->motorTorque = 0.0f;
	}

	if(glm::abs(horizontal) > 0)
	{
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

		wheels[0]->steerAngle = angleLeft * -1.0f;
		wheels[1]->steerAngle = angleRight * -1.0f;
	}
	else
	{
		wheels[0]->steerAngle = 0.0f;
		wheels[1]->steerAngle = 0.0f;
	}
}

void Car::OnFixedUpdate()
{
	if(Input::GetKeyDown(KeyCode::Space))
	{
		rigidbody->MovePosition(Vector3(0, 20, 0));
		rigidbody->MoveRotation(Quaternionf::Identity());
		rigidbody->SetAngularVelocity(Vector3(0, 0, 0));
		rigidbody->SetLinearVelocity(Vector3(0, 0, 0));
	}


}

void Car::OnGUI()
{
	float springStiffness = wheels[0]->springStiffness;
	float damperStiffness = wheels[0]->damperStiffness;
	float restLength = wheels[0]->restLength;

	ImGui::Begin("Wheel Settings");
	if(ImGui::DragFloat("Spring", &springStiffness))
	{
		for(size_t i = 0; i < wheels.size(); i++)
			wheels[i]->springStiffness = springStiffness;
			
	}
	if(ImGui::DragFloat("Damper", &damperStiffness))
	{
		for(size_t i = 0; i < wheels.size(); i++)
			wheels[i]->damperStiffness = damperStiffness;
			
	}

	if(ImGui::DragFloat("Rest Length", &restLength))
	{
		for(size_t i = 0; i < wheels.size(); i++)
			wheels[i]->restLength = restLength;
			
	}
	ImGui::End();
}