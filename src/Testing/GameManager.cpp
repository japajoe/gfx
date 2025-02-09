#include "GameManager.hpp"

void GameManager::OnInitialize()
{
	auto camera = Camera::GetMain();
	camera->SetFarClippingPlane(10000);
	camera->SetClearColor(Color::White());
	camera->GetTransform()->SetPosition(Vector3(0, 2, 10));
	camera->GetGameObject()->AddComponent<FirstPersonCamera>();
	
	GameObject::CreatePrimitive(PrimitiveType::ProceduralSkybox);
	auto cube = GameObject::CreatePrimitive(PrimitiveType::Sphere);
	cube->GetTransform()->SetPosition(Vector3(20, 250, -20));
	cube->GetTransform()->SetRotation(Quaternionf::Euler(100, 30, 20));
	cube->AddComponent<SphereCollider>();
	cube->AddComponent<Rigidbody>(100.0f);
	cube->GetComponent<MeshRenderer>()->GetMaterial<DiffuseMaterial>(0)->SetDiffuseColor(Color::Red());

	auto ground = GameObject::CreatePrimitive(PrimitiveType::Cube);
	ground->GetTransform()->SetScale(Vector3(100, 0.1f, 100));
	auto boxCollider = ground->AddComponent<BoxCollider>();
	boxCollider->SetSize(Vector3(100, 0.1f, 100));
	ground->AddComponent<Rigidbody>(0.0f);
	ground->GetComponent<MeshRenderer>()->GetMaterial<DiffuseMaterial>(0)->SetDiffuseColor(Color::LightGreen());

	auto terrainObject = GameObject::CreatePrimitive(PrimitiveType::Terrain);
	auto terrain = terrainObject->GetComponent<Terrain>();
	
	for(uint32_t y = 0; y < terrain->GetDepth() + 1; y++)
	{
		for(uint32_t x = 0; x < terrain->GetWidth() + 1; x++)
		{
			float height = Noise::GetSample(x, y) * 150.0f;
			terrain->SetHeight(x, y, height);
		}
	}

	terrain->Update();

	//terrainObject->AddComponent<TerrainCollider>();
	auto collider = terrainObject->AddComponent<MeshCollider>();
	collider->SetMesh(terrain->GetMesh(0));
	terrainObject->AddComponent<Rigidbody>(0.0f);
	
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
}

void GameManager::OnGUI()
{

}