#include "PrefabUtility.hpp"

Texture2D *PrefabUtility::CreateTexture(const std::string &name, const Color &color)
{
	return Resources::AddTexture2D(name, Texture2D(2, 2, color));
}

Texture2D *PrefabUtility::CreateSplatMap(const std::string &name, const Vector2 &size)
{
	uint32_t width = static_cast<uint32_t>(size.x);
	uint32_t height = static_cast<uint32_t>(size.y);
	uint32_t channels = 4;

	std::vector<uint8_t> pixels;
	pixels.resize(width * height * channels);

	for (uint32_t y = 0; y < height; y++)
	{
		for (uint32_t x = 0; x < width; x++)
		{
			float noiseValue = Mathf::PerlinNoise(x, y); // Scale for noise variation
			uint8_t sample = static_cast<uint8_t>(noiseValue * 255);
			uint32_t pixelIndex = (y * width + x) * channels;

			//printf("%f\n", noiseValue);

			// Splat map logic: Assign different thresholds to channels
			if (noiseValue < 0.3f)
			{
				// Channel 0 (Red): Texture 1 (e.g., dirt)
				pixels[pixelIndex + 0] = sample; 
				pixels[pixelIndex + 1] = 0;
				pixels[pixelIndex + 2] = 0;
			}
			else if (noiseValue < 0.6f)
			{
				// Channel 1 (Green): Texture 2 (e.g., grass)
				pixels[pixelIndex + 0] = 0;
				pixels[pixelIndex + 1] = sample;
				pixels[pixelIndex + 2] = 0;
			}
			else if(noiseValue < 0.85f)
			{
				// Channel 2 (Blue): Texture 3 (e.g., rock)
				pixels[pixelIndex + 0] = 0;
				pixels[pixelIndex + 1] = 0;
				pixels[pixelIndex + 2] = sample;
			}
			
			pixels[pixelIndex + 3] = 255; // Alpha channel is always opaque
		}
	}
	
	Texture2D texture(pixels.data(), pixels.size(), width, height, channels);
	return Resources::AddTexture2D(name, texture);
}

GameObject *PrefabUtility::CreateGround()
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
	return ground;
}

GameObject *PrefabUtility::CreateTerrain()
{
	auto terrainObject = GameObject::CreatePrimitive(PrimitiveType::Terrain);
	auto terrain = terrainObject->GetComponent<Terrain>();
	auto material = terrain->GetMaterial();

    float posX = -(terrain->GetWidth() * terrain->GetScale() * 0.5);
    float posZ = (terrain->GetDepth() * terrain->GetScale() * 0.5);
    terrainObject->GetTransform()->SetPosition(Vector3(posX, 0, posZ));
	//terrainObject->GetTransform()->SetPosition(Vector3(0, 0, posZ));
	const bool setHeights = true;
    
	if(setHeights)
    {
        Noise::SetFrequency(0.015f);

        for(uint32_t y = 0; y < terrain->GetDepth() + 1; y++)
        {
            for(uint32_t x = 0; x < terrain->GetWidth() + 1; x++)
            {
                float height = Noise::GetSample(x, y) * 2.0;
                height -= 1.0f;
                terrain->SetHeight(x, y, height * 5.0f);
            }
        }

        terrain->Update();
    }

	float uvScaleX = (terrain->GetWidth() * terrain->GetScale() * 0.5f * 1.0f);
	float uvScaleY = (terrain->GetDepth() * terrain->GetScale() * 0.5f * 1.0f);

	material->SetUvScale1(Vector2(uvScaleX, uvScaleY));
	material->SetUvScale2(Vector2(uvScaleX / 5.0f, uvScaleY / 5.0f));
	material->SetUvScale3(Vector2(uvScaleX, uvScaleY));
	material->SetUvScale4(Vector2(uvScaleX / 5.0f, uvScaleY / 5.0f));

	auto collider = terrainObject->AddComponent<MeshCollider>();
	collider->SetMesh(terrain->GetMesh(0));
	auto rb = terrainObject->AddComponent<Rigidbody>(0.0f);
	rb->MovePosition(Vector3(posX, 0, posZ));
	return terrainObject;
}

std::vector<GameObject*> PrefabUtility::CreateCubes()
{
	const int numCubesX = 10;
	const int numCubesY = 10;
	float spacing = 1.0f;
	float cubeSize = 1.0f;
	std::vector<GameObject*> cubes;
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
			cubes.push_back(cube);
		}
	}

	return cubes;
}

GameObject *PrefabUtility::CreateCharacter()
{
    auto character = GameObject::CreatePrimitive(PrimitiveType::Capsule);
    auto glassesLeft = GameObject::CreatePrimitive(PrimitiveType::Cube);
    auto glassesRight = GameObject::CreatePrimitive(PrimitiveType::Cube);

    character->SetName("Character");
    glassesLeft->SetName("Glasses Left");
    glassesRight->SetName("Glasses Right");

    glassesRight->GetTransform()->SetPosition(Vector3( 0.275f, 0.5f, 0.2f));
    glassesLeft->GetTransform()->SetPosition (Vector3(-0.275f, 0.5f, 0.2f));

    glassesRight->GetTransform()->SetScale(Vector3(0.5f, 0.2f, 0.6f));
    glassesLeft->GetTransform()->SetScale (Vector3(0.5f, 0.2f, 0.6f));

    glassesRight->GetTransform()->SetParent(character->GetTransform());
    glassesLeft->GetTransform()->SetParent(character->GetTransform());

    auto material = character->GetComponent<MeshRenderer>()->GetMaterial<DiffuseMaterial>(0);
    material->SetDiffuseColor(Color(18, 89, 201, 255));
    material->SetReceiveShadows((false));

    material = glassesRight->GetComponent<MeshRenderer>()->GetMaterial<DiffuseMaterial>(0);
    material->SetDiffuseColor(Color(15, 15, 15, 255));
    material->SetReceiveShadows((false));
    material = glassesLeft->GetComponent<MeshRenderer>()->GetMaterial<DiffuseMaterial>(0);
    material->SetDiffuseColor(Color(15, 15, 15, 255));
    material->SetReceiveShadows((false));
    
    return character;
}

GameObject *PrefabUtility::CreateBall()
{
	auto ballObject = GameObject::CreatePrimitive(PrimitiveType::Sphere);
	auto collider = ballObject->AddComponent<SphereCollider>();
	collider->SetRadius(0.5f);
	auto ball = ballObject->AddComponent<Rigidbody>(10.0f);
	ball->MovePosition(Vector3(0, 5, 0));
	return ballObject;
}