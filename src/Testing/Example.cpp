#include "Example.hpp"
#include "Prefab.hpp"

void Example::OnInitialize()
{
	auto camera = Camera::GetMain();
	camera->SetFarClippingPlane(10000);
	//camera->GetGameObject()->AddComponent<FirstPersonCamera>();
	camera->GetTransform()->SetPosition(Vector3(0, 2, 10));

	auto skybox = Prefab::InstantiateSkybox();

	Prefab::InstantiateTerrain(true);

	//CreateVehicle();

	auto car = GameObject::Create();
	car->AddComponent<Car>();
}

void Example::OnUpdate()
{
	GUILayout::BeginFrame();
	GUILayout::Text("FPS " + std::to_string(Time::GetFPS()));
	GUILayout::EndFrame();

	if(Input::GetKeyDown(KeyCode::C))
	{
		Input::SetMouseCursor(!Input::IsCursorVisible());
	}
}

void Example::OnFixedUpdate()
{
	// if(Input::GetKeyDown(KeyCode::Space))
	// {
	// 	rigidbody->MovePosition(Vector3(0, 20, 0));
	// 	rigidbody->MoveRotation(Quaternionf::Identity());
	// 	rigidbody->SetAngularVelocity(Vector3(0, 0, 0));
	// 	rigidbody->SetLinearVelocity(Vector3(0, 0, 0));
	// }

	// float vertical = Input::GetAxis("Vertical");
	// float horizontal = Input::GetAxis("Horizontal") * -1.0f;

	// if(glm::abs(vertical) > 0)
	// {
	// 	wheels[0]->motorTorque = vertical * 1000.0f;
	// 	wheels[1]->motorTorque = vertical * 1000.0f;
	// }
	// else
	// {
	// 	wheels[0]->motorTorque = 0.0f;
	// 	wheels[1]->motorTorque = 0.0f;
	// }

	// if(glm::abs(horizontal) > 0)
	// {
	// 	wheels[0]->steerAngle = horizontal * 30.0f;
	// 	wheels[1]->steerAngle = horizontal * 30.0f;
	// }
	// else
	// {
	// 	wheels[0]->steerAngle = 0.0f;
	// 	wheels[1]->steerAngle = 0.0f;
	// }
}

void Example::OnGUI()
{
	// float springStiffness = wheels[0]->springStiffness;
	// float damperStiffness = wheels[0]->damperStiffness;
	// float restLength = wheels[0]->restLength;

	// ImGui::Begin("Wheel Settings");
	// if(ImGui::DragFloat("Spring", &springStiffness))
	// {
	// 	for(size_t i = 0; i < wheels.size(); i++)
	// 		wheels[i]->springStiffness = springStiffness;
			
	// }
	// if(ImGui::DragFloat("Damper", &damperStiffness))
	// {
	// 	for(size_t i = 0; i < wheels.size(); i++)
	// 		wheels[i]->damperStiffness = damperStiffness;
			
	// }

	// if(ImGui::DragFloat("Rest Length", &restLength))
	// {
	// 	for(size_t i = 0; i < wheels.size(); i++)
	// 		wheels[i]->restLength = restLength;
			
	// }
	// ImGui::End();
}

// void Example::CreateVehicle()
// {
// 	auto cubeObject = GameObject::CreatePrimitive(PrimitiveType::Cube);
// 	cubeObject->GetTransform()->SetScale(Vector3(1.8f, 1.6f, 4.5f));
// 	auto cube = GameObject::Create();
// 	cubeObject->GetTransform()->SetParent(cube->GetTransform());

// 	auto collider = cube->AddComponent<BoxCollider>();
// 	collider->SetSize(Vector3(1.8f, 1.6f, 4.5f));

// 	rigidbody = cube->AddComponent<Rigidbody>(100.0f);

// 	auto createWheel = [] (const Vector3 &position, Transform *parent) -> Wheely* {
// 		auto w = GameObject::Create();
// 		w->GetTransform()->SetParent(parent);
// 		w->GetTransform()->SetPosition(position);
// 		auto col = w->AddComponent<Wheely>();
// 		col->center = Vector3(0, -0.4f, 0);
// 		return col;
// 	};

// 	auto frontLeft = createWheel(Vector3(-2, 0, -2.25f), cube->GetTransform());
// 	auto frontRight = createWheel(Vector3(2, 0, -2.25f), cube->GetTransform());
// 	auto rearLeft = createWheel(Vector3(-2, 0, 2.25f), cube->GetTransform());
// 	auto rearRight = createWheel(Vector3(2, 0, 2.25f), cube->GetTransform());

// 	wheels.reserve(4);
// 	wheels.push_back(frontLeft);
// 	wheels.push_back(frontRight);
// 	wheels.push_back(rearLeft);
// 	wheels.push_back(rearRight);

// 	rigidbody->MovePosition(Vector3(0, 10, 0));

// 	auto orbit = Camera::GetMain()->GetGameObject()->AddComponent<MouseOrbit>();
// 	orbit->SetTarget(cube->GetTransform());
// }

/////////////////////////
//////////Wheel//////////
/////////////////////////

// void Wheely::OnInitialize()
// {
// 	rb = GetTransform()->GetRoot()->GetGameObject()->GetComponent<Rigidbody>();
//     minLength = 0.0f;
//     maxLength = 0.0f;
//     lastLength = 0.0f;
//     springLength = 0.0f;
//     springVelocity = 0.0f;
// 	springForce = 0.0f;
// 	damperForce = 0.0f;
// 	suspensionForce = Vector3(0, 0, 0);
	
// 	restLength = 0.5f;
//     springTravel = 0.2f;
//     springStiffness = 2000.0f;
//     damperStiffness = 250.0f;
//     wheelRadius = 0.5f;
// 	steerAngle = 0.0f;
// 	center = Vector3(0, 0, 0);
// }

// void Wheely::OnUpdate()
// {
// 	if(!rb)
// 		return;

// 	auto transform = GetTransform();
//     Vector3 currentEuler = Quaternionf::ToEulerAngles(transform->GetLocalRotation());
//     auto rotation = Quaternion(Vector3(currentEuler.x, glm::radians(steerAngle), currentEuler.z));
// 	transform->SetLocalRotation(rotation);
// }

// void Wheely::OnFixedUpdate()
// {
// 	if(!rb)
// 		return;

// 	minLength = restLength - springTravel;
// 	maxLength = restLength + springTravel;

// 	auto transform = GetTransform();
// 	Vector3 from = transform->GetPosition() + GetWheelOffset();
// 	Vector3 direction = Vector3f::Normalize(-transform->GetUp());
// 	RaycastHit hit;

// 	if (Physics::RayTest(from, direction, maxLength + wheelRadius, hit))
// 	{
// 		if(hit.transform->GetRoot() != GetTransform()->GetRoot())
// 		{
// 			lastLength = springLength;
// 			springLength = hit.distance - wheelRadius;
// 			springLength = glm::clamp(springLength, minLength, maxLength);
// 			springVelocity = (lastLength - springLength) / Time::GetDeltaTime();
// 			springForce = springStiffness * (restLength - springLength);
// 			damperForce = damperStiffness * springVelocity;
// 			suspensionForce = (springForce + damperForce) * transform->GetUp();

// 			auto wheelVelocity = transform->InverseTransformDirection(rb->GetPointVelocity(hit.point));
// 			float fX = glm::abs(motorTorque ) > 0 ? glm::sign(motorTorque) * springForce : 0.0f;
// 			float fY = wheelVelocity.x * springForce;

// 			Vector3 frictionForce = (fX * transform->GetForward()) + (fY * -transform->GetRight());

// 			rb->AddForceAtPoint(suspensionForce + frictionForce, hit.point);
// 			isGrounded = true;
// 		}
// 		else
// 		{
// 			isGrounded = false;
// 		}
// 	}
// 	else
// 	{
// 		isGrounded = false;
// 	}

// 	if(glm::abs(motorTorque) > 0.0f)
// 	{
// 		rb->AddForceAtPoint(transform->GetForward() * motorTorque, hit.point);
// 	}

// 	DrawDebugLines();
// }

// Vector3 Wheely::GetWheelOffset() const
// {
// 	Transform *transform = GetTransform();
// 	Vector3 offset =    center.x * transform->GetRight() +
// 						center.y * transform->GetUp() +
// 						center.z * transform->GetForward();
// 	return offset;
// }

// void Wheely::DrawDebugLines()
// {
// 	Transform *transform = GetTransform();

// 	Vector3 offset = GetWheelOffset();
// 	Vector3 start = transform->GetPosition() + offset;
// 	Vector3 end = start + (-transform->GetUp() * (springLength + wheelRadius));

// 	Color color = isGrounded ? Color::Red() : Color::Green();
// 	Debug::DrawLine(start, end, color);
	
// 	Quaternion rotation = transform->GetRotation();
// 	DrawCircle(start, wheelRadius, rotation);

// 	//end = start + (forceDirection * 2.0f);
	
// 	Debug::DrawLine(start, end, color);
// }

// void Wheely::DrawCircle(const Vector3 &center, float radius, const Quaternion &rotation)
// {
// 	int segments = 24;

// 	Quaternion additionalRotation = Quaternionf::FromAxisAngle(Vector3f::UnitY(), glm::radians(90.0f)); // add 90 degrees around the y-axis
// 	auto rot = rotation * additionalRotation; // apply the additional rotation        

// 	Vector3 xVector = rot * Vector3f::UnitX() * radius;
// 	Vector3 yVector = rot * Vector3f::UnitY() * radius;

// 	for (int i = 0; i < segments; i++)
// 	{
// 		float angle = i * glm::pi<float>() * 2 / segments;
// 		float nextAngle = (i + 1) * glm::pi<float>() * 2 / segments;

// 		Vector3 start = center + glm::cos(angle) * xVector + glm::sin(angle) * yVector;
// 		Vector3 end = center + glm::cos(nextAngle) * xVector + glm::sin(nextAngle) * yVector;

// 		Debug::DrawLine(start, end, Color::White());
// 	}
// }