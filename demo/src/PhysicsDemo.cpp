#include "PhysicsDemo.hpp"

void PhysicsDemo::OnInitialize()
{
	font = Resources::FindFont(Constants::GetString(ConstantString::FontDefault));

	World::SetFogEnabled(false);

	auto skybox = GameObject::CreatePrimitive(PrimitiveType::ProceduralSkybox2);
	auto camera = Camera::GetMain();
	camera->SetFarClippingPlane(10000);
	camera->GetGameObject()->AddComponent<FirstPersonCamera>();
	camera->GetGameObject()->GetTransform()->SetPosition(Vector3(0, 2, 10));

	auto lightTransform = Light::GetMain()->GetTransform();
	lightTransform->SetRotation(Quaternionf::Euler(glm::radians(5.0f), 0, 0));
	
	auto material = skybox->GetComponent<MeshRenderer>()->GetMaterial<ProceduralSkybox2Material>(0);
	material->SetSunPositionFromLightDirection(lightTransform->GetForward());
	material->SetExposure(0.3f);

	CreateGround();
	CreateBall();
	CreateCubes();

	camera->GetGameObject()->AddComponent<AudioListener>();
}

void PhysicsDemo::OnUpdate()
{
	if(Input::GetKeyDown(KeyCode::Escape))
	{
		Application::Quit();
	}

	if(Input::GetKeyDown(KeyCode::C))
	{
		Input::SetMouseCursor(!Input::IsCursorVisible());
	}

	if(Input::GetKeyDown(KeyCode::P))
	{
		ResetCubes();
	}

	if(Input::GetKeyDown(KeyCode::Space))
	{
		ball->SetLinearVelocity(Vector3f::Zero());
		ball->SetAngularVelocity(Vector3f::Zero());
		ball->MovePosition(Camera::GetMain()->GetTransform()->GetPosition());
		ball->SetLinearVelocity(Camera::GetMain()->GetTransform()->GetForward() * 50.0f);
	}

	std::string fps = "FPS " + std::to_string(Time::GetFPS());
	Graphics2D::AddText(Vector2(5, 5), font, fps, 14, Color::Black(), false);
}

void PhysicsDemo::OnGUI()
{

}

void PhysicsDemo::CreateGround()
{
	auto ground = GameObject::Create();
	auto mesh = std::make_shared<Mesh>(MeshGenerator::CreatePlane(Vector3(10000, 1, 10000)));
	auto material = std::make_shared<DiffuseMaterial>();
	auto texture = Resources::FindTexture2D(Constants::GetString(ConstantString::TextureDefaultGrass));
	material->SetDiffuseTexture(texture);
	material->SetDiffuseColor(Color(152, 210, 124, 255));
	material->SetUVScale(Vector2(10000, 10000));
	auto renderer = ground->AddComponent<MeshRenderer>();
	renderer->Add(mesh, material);
	auto collider = ground->AddComponent<MeshCollider>();
	collider->SetMesh(renderer->GetMesh(0));
	ground->AddComponent<Rigidbody>(0);
}

void PhysicsDemo::CreateBall()
{
	auto ballObject = GameObject::CreatePrimitive(PrimitiveType::Sphere);
	auto collider = ballObject->AddComponent<SphereCollider>();
	collider->SetRadius(0.5f);
	ball = ballObject->AddComponent<Rigidbody>(10.0f);
	ball->MovePosition(Vector3(0, 5, 5));
}

void PhysicsDemo::CreateCubes()
{
	float spacing = 1.0f;
	float cubeSize = 1.0f;
	cubes.reserve(numCubesX * numCubesY);

	for (int i = 0; i < numCubesX; ++i)
	{
		for (int j = 0; j < numCubesY; j++)
		{
			auto cube = GameObject::CreatePrimitive(PrimitiveType::Cube);
			auto renderer = cube->GetComponent<MeshRenderer>();
			auto material = renderer->GetMaterial<DiffuseMaterial>(0);
			material->SetDiffuseColor(Color(Random::Range(0.0f, 1.0f), Random::Range(0.0f, 1.0f), Random::Range(0.0f, 1.0f)));
			auto collider = cube->AddComponent<BoxCollider>();
			collider->SetSize(Vector3(cubeSize, cubeSize, cubeSize));
			auto rb = cube->AddComponent<Rigidbody>(1.0f);
			rb->MovePosition(Vector3(i * spacing, j * spacing + cubeSize/2 , 0));
			cubes.push_back(rb);
		}
	}
}

void PhysicsDemo::ResetCubes()
{
	float spacing = 1.0f;
	float cubeSize = 1.0f;
	
	for (int i = 0; i < numCubesX; ++i)
	{
		for (int j = 0; j < numCubesY; j++)
		{
			auto rb = cubes[i * numCubesY + j];
			rb->SetLinearVelocity(Vector3f::Zero());
			rb->SetAngularVelocity(Vector3f::Zero());
			rb->MovePosition(Vector3(i * spacing, j * spacing + cubeSize/2 , 0));
			rb->MoveRotation(Quaternionf::Identity());
		}
	}
}
