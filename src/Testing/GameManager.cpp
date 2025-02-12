#include "GameManager.hpp"

void GameManager::OnInitialize()
{
	auto camera = Camera::GetMain();
	camera->SetFarClippingPlane(10000);
	camera->SetClearColor(Color::White());
	camera->GetTransform()->SetPosition(Vector3(0, 2, 10));
	//camera->GetGameObject()->AddComponent<FirstPersonCamera>();

	GameObject::CreatePrimitive(PrimitiveType::ProceduralSkybox);

	CreateBall();
	//CreateCube();
	CreateTerrain();

	//auto rustler = ModelImporter::LoadFromFile("../res/Models/Rustler/Rustler.fbx", ModelFlags_FlipUVs, Vector3(1, 1, 1));
	
}

void GameManager::OnUpdate()
{
	if(Input::GetKeyDown(KeyCode::Escape))
	{
		Application::Quit();
	}

	if(Input::GetKeyDown(KeyCode::C))
	{
		Input::SetMouseCursor(!Input::IsCursorVisible());
	}


	if(rb != nullptr)
	{
		if(Input::GetKeyDown(KeyCode::R))
		{
			rb->SetLinearVelocity(Vector3f::Zero());
			rb->SetAngularVelocity(Vector3f::Zero());
			rb->MoveRotation(Quaternionf::Identity());
		}

		if(Input::GetKeyDown(KeyCode::P))
		{
			rb->GetGameObject()->SetIsActive(!rb->GetGameObject()->GetIsActive());
		}

		auto pos = rb->GetTransform()->GetPosition();
		auto str = Vector3f::ToString(pos);
		GUI::BeginFrame();
		GUI::Button(Rectangle(10, 10, 300, 20), str);
		GUI::EndFrame();
	}
}

void GameManager::OnFixedUpdate()
{
	if(Input::GetKey(KeyCode::Space))
	{
		rb->AddForce(Vector3f::UnitY() * 10000.0f);
	}

	float vertical = Input::GetAxis("Vertical");
	float horizontal = Input::GetAxis("Horizontal");

	if(glm::abs(vertical) > 0 || glm::abs(horizontal) > 0)
	{
		// Get Camera Normalized Directional Vectors
		Vector3 forward = Camera::GetMain()->GetTransform()->GetForward();
		Vector3 right = Camera::GetMain()->GetTransform()->GetRight();
		forward.y = 0;
		right.y = 0;
		forward = Vector3f::Normalize(forward);
		right = Vector3f::Normalize(right);

		// Create direction-relative-input vectors
		Vector3 forwardRelativeVerticalInput = vertical * forward;
		Vector3 rightRelativeHorizontalInput = horizontal * right;

		// Create and apply camera relative movement
		Vector3 cameraRelativeMovement = Vector3f::Normalize(forwardRelativeVerticalInput + rightRelativeHorizontalInput);

		rb->AddForce(cameraRelativeMovement * 5000.0f);
	}

}

void GameManager::OnGUI()
{

}

void GameManager::CreateBall()
{
	auto orbit = Camera::GetMain()->GetGameObject()->AddComponent<MouseOrbit>();
	auto sphere = GameObject::CreatePrimitive(PrimitiveType::Sphere);
	sphere->GetTransform()->SetPosition(Vector3(20, 30, -20));
	sphere->AddComponent<SphereCollider>();
	rb = sphere->AddComponent<Rigidbody>(100.0f);
	rb->SetBounciness(0.35f);
	sphere->GetComponent<MeshRenderer>()->GetMaterial<DiffuseMaterial>(0)->SetDiffuseColor(Color::Orange());
	orbit->SetTarget(sphere->GetTransform());
}

void GameManager::CreateCube()
{
	auto orbit = Camera::GetMain()->GetGameObject()->AddComponent<MouseOrbit>();
	auto cube = GameObject::CreatePrimitive(PrimitiveType::Cube);
	cube->GetTransform()->SetPosition(Vector3(20, 30, -20));
	Vector3 size(1.8f, 1.6f, 4.2f);
	cube->GetTransform()->SetScale(size);
	auto collider = cube->AddComponent<BoxCollider>();
	collider->SetSize(size);
	rb = cube->AddComponent<Rigidbody>(100.0f);
	rb->SetBounciness(0);
	cube->GetComponent<MeshRenderer>()->GetMaterial<DiffuseMaterial>(0)->SetDiffuseColor(Color::Orange());
	orbit->SetTarget(cube->GetTransform());

	auto wheelFronLeft = cube->AddComponent<Wheel>();
	auto wheelFrontRight = cube->AddComponent<Wheel>();
	auto wheelRearLeft = cube->AddComponent<Wheel>();
	auto wheelRearRight = cube->AddComponent<Wheel>();

	wheelFronLeft->SetCenter(Vector3(size.x * 1.0 * -1.0f,  -1.0f, size.z * -1.2f));
	wheelFrontRight->SetCenter(Vector3(size.x * 1.0, 		-1.0f, size.z * -1.2f));

	wheelRearLeft->SetCenter(Vector3(size.x * 1.0 * -1.0f,  -1.0f, size.z * 1.2f));
	wheelRearRight->SetCenter(Vector3(size.x * 1.0, 		-1.0f, size.z * 1.2f));
}

void GameManager::CreateTerrain()
{
	auto terrainObject = GameObject::CreatePrimitive(PrimitiveType::Terrain);
	auto terrain = terrainObject->GetComponent<Terrain>();	
	auto material = terrain->GetMaterial();

    float posX = -(terrain->GetWidth() * terrain->GetScale() * 0.5);
    float posZ = (terrain->GetDepth() * terrain->GetScale() * 0.5);
    terrainObject->GetTransform()->SetPosition(Vector3(posX, 0, posZ));
	//terrainObject->GetTransform()->SetPosition(Vector3(0, 0, posZ));
	
	Noise::SetFrequency(0.015);

	for(uint32_t y = 0; y < terrain->GetDepth() + 1; y++)
	{
		for(uint32_t x = 0; x < terrain->GetWidth() + 1; x++)
		{
			float height = Noise::GetSample(x, y) * 2.0;
			height -= 1.0f;
			terrain->SetHeight(x, y, height * 25.0f);
		}
	}

	terrain->Update();

	material->SetSplatMap(LoadTexture("../res/Textures/Splatmap.jpg"));
	material->SetTexture1(LoadTexture("../res/Textures/Mud.png"));
	material->SetTexture2(LoadTexture("../res/Textures/coast_sand_rocks_02_diff_1k.jpg"));
	material->SetTexture3(LoadTexture("../res/Textures/forrest_ground_01_diff_1k.jpg"));
	material->SetTexture4(LoadTexture("../res/Textures/Grass.jpg"));

	float uvScaleX = (terrain->GetWidth() * terrain->GetScale() * 0.5f * 1.0f);
	float uvScaleY = (terrain->GetDepth() * terrain->GetScale() * 0.5f * 1.0f);

	material->SetUvScale1(Vector2(uvScaleX, uvScaleY));
	material->SetUvScale2(Vector2(uvScaleX / 5.0f, uvScaleY / 5.0f));
	material->SetUvScale3(Vector2(uvScaleX, uvScaleY));
	material->SetUvScale4(Vector2(uvScaleX / 5.0f, uvScaleY / 5.0f));

	terrainObject->AddComponent<TerrainCollider>();
	//auto collider = terrainObject->AddComponent<MeshCollider>();
	//collider->SetMesh(terrain->GetMesh(0));
	auto rb = terrainObject->AddComponent<Rigidbody>(0.0f);
	rb->MovePosition(Vector3(posX, 0, posZ));
}

Texture2D *GameManager::LoadTexture(const std::string &filepath)
{
	Image image(filepath);
	if(image.IsLoaded())
	{
		return Resources::AddTexture2D(filepath, Texture2D(&image));
	}
	Debug::WriteLine("Failed " + filepath);
	return nullptr;
}