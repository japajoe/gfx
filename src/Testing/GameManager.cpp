#include "GameManager.hpp"

void GameManager::OnInitialize()
{
	auto camera = Camera::GetMain();
	camera->SetFarClippingPlane(10000);
	camera->SetClearColor(Color::White());
	camera->GetTransform()->SetPosition(Vector3(0, 2, 10));
	camera->GetGameObject()->AddComponent<FirstPersonCamera>();
	
	//auto skybox = GameObject::CreatePrimitive(PrimitiveType::ProceduralSkybox);


	
	font = Resources::FindFont(Constants::GetString(ConstantString::FontDefault));

	LoadResources();
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

	if(!cube)
		return;

	float y = Time::GetTime();
	float z = Time::GetTime();
	auto rotation = Quaternionf::Euler(0, y, y);
	cube->GetTransform()->SetRotation(rotation);

	std::string fps = std::to_string(Time::GetFPS());

	GUI::BeginFrame();
	GUI::Button(Rectangle(10, 10, 100, 20), fps);
	GUI::EndFrame();

	//Graphics2D::AddText(Vector2(10, 10), font, fps, 22, Color::White(), false);

	if(Input::GetButtonDown(ButtonCode::Left))
	{
		Ray ray = Ray::FromMousePosition(Input::GetMousePosition(), Graphics::GetViewport());
		RaycastHit hit;
		if(Physics::Raycast(ray, hit))
		{
			Debug::WriteLine("Hit object: " + hit.transform->GetGameObject()->GetName());
		}
	}
}

void GameManager::OnGUI()
{

}

void GameManager::LoadResources()
{
	auto textureBatch = std::vector<std::string>
	{
		"Resources/Textures/Box.jpg",
		"Resources/Textures/Grass.jpg"
	};

	// auto fontBatch = std::vector<std::string>
	// {
	// 	"Resources/Fonts/SF Sports Night.ttf"
	// };

	// auto audioBatch = std::vector<std::string>
	// {
	// 	"Resources/Audio/unhappy-drone-67284.wav",
	// 	"Resources/Audio/click1.mp3",
	// 	"Resources/Audio/click2.mp3",
	// 	"Resources/Audio/Fire.wav"
	// };

	auto skyboxBatch = std::vector<std::string>
	{
		"Resources/Textures/Skyboxes/Sahara/right.png",
		"Resources/Textures/Skyboxes/Sahara/left.png",
		"Resources/Textures/Skyboxes/Sahara/bottom.png",
		"Resources/Textures/Skyboxes/Sahara/top.png",
		"Resources/Textures/Skyboxes/Sahara/front.png",
		"Resources/Textures/Skyboxes/Sahara/back.png"
	};

	// auto shaderBatch = std::vector<std::string>
	// {
	// 	"Resources/Shaders/BasicV.glsl",
	// 	"Resources/Shaders/FireF.glsl"
	// };

	totalResourceCount = 0;
	totalResourceCount += skyboxBatch.size();
	totalResourceCount += textureBatch.size();
	// totalResourceCount += fontBatch.size();
	// totalResourceCount += audioBatch.size();
	// totalResourceCount += shaderBatch.size();

	Resources::LoadAsyncBatchFromAssetPack(ResourceType::TextureCubeMap, skyboxBatch, "../res/assets.dat", "assets.dat");
	Resources::LoadAsyncBatchFromAssetPack(ResourceType::Texture2D, textureBatch, "../res/assets.dat", "assets.dat");
	// Resources::LoadAsyncBatchFromAssetPack(ResourceType::Font, fontBatch, "../res/assets.dat", "assets.dat");
	// Resources::LoadAsyncBatchFromAssetPack(ResourceType::AudioClip, audioBatch, "../res/assets.dat", "assets.dat");
	// Resources::LoadAsyncBatchFromAssetPack(ResourceType::Shader, shaderBatch, "../res/assets.dat", "assets.dat");
}

void GameManager::OnResourceBatchLoadedAsync(const ResourceBatch &batch)
{
	switch(batch.type)
	{
		case ResourceType::TextureCubeMap:
		{
			std::vector<Image*> images;

			for(int i = 0; i < batch.resources.size(); i++)
			{
				images.push_back(new Image(batch.resources[i].data.data(), batch.resources[i].data.size()));
			}

			TextureCubeMap *texture = Resources::AddTextureCubeMap("Skybox", TextureCubeMap(images));
			auto skybox = GameObject::CreatePrimitive(PrimitiveType::Skybox);
			auto mat = skybox->GetComponent<MeshRenderer>()->GetMaterial<SkyboxMaterial>(0);
			mat->SetDiffuseTexture(texture);

			for(int i = 0; i < images.size(); i++)
			{
				delete images[i];
			}
			break;
		}
		case ResourceType::Texture2D:
		{
			for(int i = 0; i < batch.resources.size(); i++)
			{
				Image image(batch.resources[i].data.data(), batch.resources[i].data.size());
				Resources::AddTexture2D(batch.resources[i].name, Texture2D(&image));
			}

			break;
		}
	}

	currentResourceCount += batch.resources.size();

	if(currentResourceCount == totalResourceCount)
	{
		OnLoadingComplete();
	}

}

void GameManager::OnLoadingComplete()
{
	auto textureBox = Resources::FindTexture2D("Resources/Textures/Box.jpg");
	auto textureGrass = Resources::FindTexture2D("Resources/Textures/Grass.jpg");

	cube = GameObject::CreatePrimitive(PrimitiveType::Cube);
	cube->GetTransform()->SetPosition(Vector3(0, 2, 0));
	cube->GetTransform()->SetScale(Vector3(1, 1, 1));
	auto cubeMaterial = cube->GetComponent<MeshRenderer>()->GetMaterial<DiffuseMaterial>(0);
	cubeMaterial->SetDiffuseTexture(textureBox);
	cube->SetName("Cube");

	auto plane = GameObject::CreatePrimitive(PrimitiveType::Plane);
	plane->GetTransform()->SetScale(Vector3(1000, 1, 1000));
	auto planeMaterial = plane->GetComponent<MeshRenderer>()->GetMaterial<DiffuseMaterial>(0);
	planeMaterial->SetDiffuseTexture(textureGrass);
	planeMaterial->SetUVScale(Vector2(200, 200));
	plane->SetName("Plane");
}