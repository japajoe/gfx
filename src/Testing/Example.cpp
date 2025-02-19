#include "Example.hpp"
#include "Prefab.hpp"

void Example::OnInitialize()
{
	auto camera = Camera::GetMain();
	camera->SetFarClippingPlane(10000);
	camera->GetGameObject()->AddComponent<FirstPersonCamera>();
	camera->GetTransform()->SetPosition(Vector3(0, 2, 10));

	auto skybox = Prefab::InstantiateSkybox();
	skyboxMaterial = skybox->GetComponent<MeshRenderer>()->GetMaterial<ProceduralSkybox2Material>(0);

	//Prefab::InstantiateTerrain(false);

	// const size_t rows = 10;
	// const size_t cols = 10;

	// Image image("../res/Box.jpg");
	// Texture2D *texture = nullptr;
	// if(image.IsLoaded())
	// {
	// 	texture = Resources::AddTexture2D("../res/Box.jpg", Texture2D(&image));
	// }

	// for(size_t y = 0; y < rows; y++)
	// {
	// 	for(size_t x = 0; x < cols; x++)
	// 	{
	// 		GameObject *cube = GameObject::CreatePrimitive(PrimitiveType::Cube);
	// 		auto material = cube->GetComponent<MeshRenderer>()->GetMaterial<DiffuseMaterial>(0);
	// 		material->SetDiffuseTexture(texture);
	// 		cube->AddComponent<BoxCollider>();
	// 		auto rb = cube->AddComponent<Rigidbody>(10.0f);
	// 		rb->MovePosition(Vector3(x, y+1, 0));
	// 		cubes.push_back(rb);
	// 	}
	// }

	// auto ballObject = GameObject::CreatePrimitive(PrimitiveType::Sphere);
	// ballObject->AddComponent<SphereCollider>();
	// ball = ballObject->AddComponent<Rigidbody>(500.0f);
	// ball->MovePosition(Vector3(0, 5, 0));
}

void Example::OnUpdate()
{
	// if(Input::GetKeyDown(KeyCode::P))
	// {
	// 	ResetCubes();
	// }

	// if(Input::GetKeyDown(KeyCode::Space))
	// {
	// 	ShootBall();
	// }

	// GUILayout::BeginFrame();
	// GUILayout::Text("FPS " + std::to_string(Time::GetFPS()));
	// GUILayout::EndFrame();
}

void Example::OnGUI()
{
	// auto light = Light::GetMain();

	// auto rotation = Quaternionf::ToEulerAngles(light->GetTransform()->GetRotation());
	// rotation.x = glm::degrees(rotation.x);
	// rotation.y = glm::degrees(rotation.y);
	// rotation.z = glm::degrees(rotation.z);

	// ImGui::Begin("Light");
	// if(ImGui::DragFloat3("Rotation", &rotation.x))
	// {
    // 	float pitch = glm::radians(rotation.x); // Rotation around X-axis
    // 	float yaw = glm::radians(rotation.y);   // Rotation around Y-axis
    // 	float roll = glm::radians(rotation.z);  // Rotation around Z-axis

	// 	Quaternion rotation(Vector3(pitch, yaw, roll));

	// 	light->GetTransform()->SetRotation(rotation);
	// 	skyboxMaterial->SetSunPositionFromLightDirection(light->GetTransform()->GetForward());
	// }

	// ImGui::End();
}

void Example::ResetCubes()
{
	const size_t rows = 10;
	const size_t cols = 10;

	for(size_t y = 0; y < rows; y++)
	{
		for(size_t x = 0; x < cols; x++)
		{
			size_t index = y * cols + x;
			cubes[index]->MovePosition(Vector3(x, y+1, 0));
			cubes[index]->MoveRotation(Quaternionf::Identity());
			cubes[index]->SetLinearVelocity(Vector3f::Zero());
			cubes[index]->SetAngularVelocity(Vector3f::Zero());
		}
	}
}

void Example::ShootBall()
{
	auto camera = Camera::GetMain()->GetTransform();
	auto forward = camera->GetForward();
	auto pos = camera->GetPosition() + camera->GetForward() * 1.0f;
	ball->MovePosition(pos);
	ball->MoveRotation(Quaternionf::Identity());
	ball->SetLinearVelocity(Vector3f::Zero());
	ball->SetAngularVelocity(Vector3f::Zero());

	ball->SetLinearVelocity(forward * 25.0f);
}