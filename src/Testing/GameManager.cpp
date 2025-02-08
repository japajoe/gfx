#include "GameManager.hpp"

void GameManager::OnInitialize()
{
	auto camera = Camera::GetMain();
	camera->SetFarClippingPlane(10000);
	camera->SetClearColor(Color::White());
	camera->GetTransform()->SetPosition(Vector3(0, 2, 10));
	camera->GetGameObject()->AddComponent<FirstPersonCamera>();
	
	GameObject::CreatePrimitive(PrimitiveType::ProceduralSkybox);
	GameObject::CreatePrimitive(PrimitiveType::Cube);
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