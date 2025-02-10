#include "GameManager.hpp"

void GameManager::OnInitialize()
{
	auto camera = Camera::GetMain();
	camera->SetFarClippingPlane(10000);
	camera->SetClearColor(Color::White());
	camera->GetTransform()->SetPosition(Vector3(0, 2, 10));
	//camera->GetGameObject()->AddComponent<FirstPersonCamera>();
	auto orbit = camera->GetGameObject()->AddComponent<MouseOrbit>();
	
	GameObject::CreatePrimitive(PrimitiveType::ProceduralSkybox);
	auto cube = GameObject::CreatePrimitive(PrimitiveType::Sphere);
	cube->GetTransform()->SetPosition(Vector3(20, 30, -20));
	cube->GetTransform()->SetRotation(Quaternionf::Euler(100, 30, 20));
	cube->AddComponent<SphereCollider>();
	rb = cube->AddComponent<Rigidbody>(100.0f);
	cube->GetComponent<MeshRenderer>()->GetMaterial<DiffuseMaterial>(0)->SetDiffuseColor(Color::Orange());
	orbit->SetTarget(cube->GetTransform());


	CreateTerrain();
	
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
		auto pos = rb->GetTransform()->GetPosition();
		auto str = Vector3f::ToString(pos);
		GUI::BeginFrame();
		GUI::Button(Rectangle(10, 10, 300, 20), str);

		Vector3 from = rb->GetTransform()->GetPosition() + Vector3(2, 0, 0);
		Vector3 to = from + (Vector3(0, -1, 0) * 10.0f);

		Debug::DrawLine(from, to, Color::Red());

		RaycastHit hit;
		if(Physics::RayTest(from, Vector3(0, -1, 0), 10.0f, hit))
		{
			GUI::Button(Rectangle(10, 40, 300, 20), "Hit");
			// auto pos =  rb->GetTransform()->GetPosition();
			// pos.y = hit.point.y;
			// rb->MovePosition(pos);
		}
		
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

void GameManager::CreateTerrain()
{
	auto terrainObject = GameObject::CreatePrimitive(PrimitiveType::Terrain);
	auto terrain = terrainObject->GetComponent<Terrain>();
	auto material = terrain->GetMaterial();
	
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
	auto rb = terrainObject->AddComponent<Rigidbody>(0.0f);
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