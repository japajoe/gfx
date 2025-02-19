#include "Prefab.hpp"

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

GameObject *Prefab::InstantiateTerrain(bool setHeights)
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

	terrainObject->AddComponent<TerrainCollider>();
	//auto collider = terrainObject->AddComponent<MeshCollider>();
	//collider->SetMesh(terrain->GetMesh(0));
	auto rb = terrainObject->AddComponent<Rigidbody>(0.0f);
	rb->MovePosition(Vector3(posX, 0, posZ));
	return terrainObject;
}

GameObject *Prefab::InstantiateSkybox()
{
    auto skybox = GameObject::CreatePrimitive(PrimitiveType::ProceduralSkybox2);
    auto material = skybox->GetComponent<MeshRenderer>()->GetMaterial<ProceduralSkybox2Material>(0);
    material->SetExposure(0.5f);
    return skybox;
}