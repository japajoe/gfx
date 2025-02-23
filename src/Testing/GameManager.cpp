#include "GameManager.hpp"

static Texture2D *LoadTexture(const std::string &filepath)
{
	Image image(filepath);
	if(image.IsLoaded())
	{
		return Resources::AddTexture2D(filepath, Texture2D(&image));
	}
	Debug::WriteLine("Failed " + filepath);
	return nullptr;
}

static GameObject *InstantiateTerrain(bool setHeights)
{
	auto terrainObject = GameObject::CreatePrimitive(PrimitiveType::Terrain);
	auto terrain = terrainObject->GetComponent<Terrain>();
	auto material = terrain->GetMaterial();

    float posX = -(terrain->GetWidth() * terrain->GetScale() * 0.5);
    float posZ = (terrain->GetDepth() * terrain->GetScale() * 0.5);
    terrainObject->GetTransform()->SetPosition(Vector3(posX, 0, posZ));
	//terrainObject->GetTransform()->SetPosition(Vector3(0, 0, posZ));
	
    if(setHeights)
    {
        Noise::SetFrequency(0.015f);

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
    }

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

	//terrainObject->AddComponent<TerrainCollider>();
	auto collider = terrainObject->AddComponent<MeshCollider>();
	collider->SetMesh(terrain->GetMesh(0));
	auto rb = terrainObject->AddComponent<Rigidbody>(0.0f);
	rb->MovePosition(Vector3(posX, 0, posZ));
	return terrainObject;
}

void GameManager::OnInitialize()
{
    Input::SetMouseCursor(false);

    World::SetFogColor(Color(0x7c, 0x78, 0x7a, 255));

    auto camera = Camera::GetMain();
    camera->SetFarClippingPlane(10000);
    camera->GetTransform()->SetPosition(Vector3(0, 3, 0));
    camera->GetGameObject()->AddComponent<AudioListener>();

    //Light::GetMain()->GetTransform()->SetRotation(Quaternionf::Euler(glm::radians(5.0f), 0, 0));
    auto skybox = GameObject::CreatePrimitive(PrimitiveType::ProceduralSkybox2);
    // auto mat = skybox->GetComponent<MeshRenderer>()->GetMaterial<ProceduralSkybox2Material>(0);
    // mat->SetSunPositionFromLightDirection(Light::GetMain()->GetTransform()->GetForward());
    // mat->SetExposure(0.2f);

    //SpawnTerrain();
    InstantiateTerrain(false);
    SpawnVehicle();

    // ModelFlags flags = ModelFlags_Triangulate | ModelFlags_FlipUVs;
    // auto city = ModelImporter::LoadFromFile("../res/Models/City/City.fbx", flags, Vector3f::One() * 0.01f);
    // city->GetTransform()->SetPosition(Vector3(0, 0.15f, 0));
}

void GameManager::OnUpdate()
{
    std::string fps = "FPS " +  std::to_string(Time::GetFPS());
	std::string speed = std::to_string(vehicle->GetSpeed());
    GUILayout::BeginFrame();
    GUILayout::Text(fps);
	GUILayout::Text("Speed " + speed + " km/h");
    GUILayout::EndFrame();

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
        if(vehicle)
			vehicle->Flip();
    }
}

void GameManager::OnGUI()
{

}

void GameManager::SpawnTerrain()
{
    auto floor = GameObject::Create();
    auto mesh = std::make_shared<Mesh>(MeshGenerator::CreatePlane(Vector3(10000, 1, 10000)));
    auto renderer = floor->AddComponent<MeshRenderer>();
    renderer->Add(mesh, std::make_shared<DiffuseMaterial>());
    auto texture = LoadTexture("../res/Textures/Grass.jpg");
    auto material = floor->GetComponent<MeshRenderer>()->GetMaterial<DiffuseMaterial>(0);
    material->SetUVScale(Vector2(1000, 1000));
    material->SetDiffuseTexture(texture);
    auto collider = floor->AddComponent<MeshCollider>();
    collider->SetMesh(renderer->GetMesh(0));
    floor->AddComponent<Rigidbody>(0);
}

void GameManager::SpawnVehicle()
{
    ModelFlags flags = ModelFlags_Triangulate | ModelFlags_FlipUVs;
    auto model = ModelImporter::LoadFromFile("../res/Models/Mustang/Mustang.fbx", flags, Vector3f::One() * 0.01f);
    
    auto texture = LoadTexture("../res/Models/Mustang/Textures/AFRC_Tex_Col2.png");
    auto renderers = model->GetComponentsOfTypeInChildren<MeshRenderer>();
    for(size_t i = 0; i < renderers.size(); i++)
    {
        DiffuseMaterial *material = nullptr;
        size_t index = 0;
        while((material = renderers[i]->GetMaterial<DiffuseMaterial>(index++)) != nullptr)
        {
            material->SetDiffuseTexture(texture);
        }
    }

    vehicle = model->AddComponent<Vehicle>();
    VehicleSettings settings;
    settings.colliderPosition = Vector3(0, 0.9f, 0);
    settings.colliderSize = Vector3(1.6f, 1.2f, 4.8f);
    settings.mass = 1000.0f;
    settings.turnRadius = 10.8f;
    settings.antiRoll = 5000.0f;

    auto frontLeft = model->GetTransform()->FindChild("FrontLeftWheel");
    auto frontRight = model->GetTransform()->FindChild("FrontRightWheel");
    auto rearLeft = model->GetTransform()->FindChild("RearLeftWheel");
    auto rearRight = model->GetTransform()->FindChild("RearRightWheel");

    auto createWheelCollider = [this] (Transform *t) -> WheelInfo {
        GameObject *colliderObject = GameObject::Create();
        colliderObject->GetTransform()->SetPosition(t->GetPosition());
        colliderObject->GetTransform()->SetParent(t->GetRoot());
        auto collider = colliderObject->AddComponent<WheelCollider>();
        collider->SetWheelRadius(0.35f);
        collider->SetSpringStiffness(20000);
        collider->SetDamperStiffness(2000);
        //collider->SetRestLength(0.12f);
        collider->SetRestLength(0.175f);
        collider->SetDebugLines(true);
        WheelInfo wheel;
        wheel.collider = collider;
        wheel.visual = t;
        return wheel;
    };

    settings.frontLeft = createWheelCollider(frontLeft);
    settings.frontRight = createWheelCollider(frontRight);
    settings.rearLeft = createWheelCollider(rearLeft);
    settings.rearRight = createWheelCollider(rearRight);

    vehicle->Configure(settings);

    auto orbit = Camera::GetMain()->GetGameObject()->AddComponent<MouseOrbit>();
    orbit->SetTarget(model->GetTransform());
    orbit->zAxisDistance *= 0.25f;

    auto rb = model->GetComponent<Rigidbody>();
    rb->MovePosition(Vector3(0, 35, 0));
}