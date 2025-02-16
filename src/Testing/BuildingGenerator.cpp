#include "BuildingGenerator.hpp"

void BuildingGenerator::OnInitialize()
{
	auto camera = Camera::GetMain();
	camera->GetGameObject()->AddComponent<FirstPersonCamera>();

	GameObject::CreatePrimitive(PrimitiveType::ProceduralSkybox);

	CreateTerrain();

	building = GameObject::CreatePrimitive(PrimitiveType::Plane);
	renderer = building->GetComponent<MeshRenderer>();
	auto material = renderer->GetMaterial<DiffuseMaterial>(0);
	//material->SetDiffuseColor(Color::Gray());
	material->SetDiffuseTexture(Resources::FindTexture2D("../res/Textures/Mud.png"));

	material->SetReceiveShadows(false);
	mesh = renderer->GetMesh(0);
}

void BuildingGenerator::OnUpdate()
{
	if(Input::GetKeyDown(KeyCode::Space))
	{
		Generate();
	}
}

void BuildingGenerator::OnGUI()
{

}

void BuildingGenerator::Generate()
{
	int sides = Random::Range(3, 12);
	float radius = Random::Range(15, 30);
	float height = 3 + (glm::round(Random::Range(0, 36) * 3));
	auto polygon = GenerateRandomPolygon2(sides, radius);
	GenerateBuildingMesh(polygon, height);
}

std::vector<Vector2> BuildingGenerator::GenerateRandomPolygon(int sides, float radius)
{
	std::vector<Vector2> polygon;
	float angleStep = 2 * glm::pi<float>() / sides;

	// Randomize vertices within a bounded area (e.g., within a circle)
	for (int i = 0; i < sides; ++i)
	{
		float angle = angleStep * i;
		float randomRadius = radius * (0.8f + 0.4f *Random::GetNextDouble());
		polygon.push_back({randomRadius * glm::cos(angle), randomRadius * glm::sin(angle)});
	}

	return polygon;
}

std::vector<Vector2> BuildingGenerator::GenerateRandomPolygon2(int sides, float radius) {
	std::vector<Vector2> footprint;

	// Ensure even number of sides >= 4
	sides = std::max(4, sides);
	if (sides % 2 != 0) sides += 1;

	// Generate base rectangle dimensions
	float width = radius * Random::Range(0.5f, 1.5f);
	float height = radius * Random::Range(0.5f, 1.5f);

	// Create initial rectangle
	footprint = {
		{ -width, -height }, // Bottom-left
		{  width, -height }, // Bottom-right
		{  width,  height }, // Top-right
		{ -width,  height }  // Top-left
	};

	// Calculate number of extrusions needed
	int extrusions = (sides - 4) / 2;

	// Precompute all extrusion points before modifying the footprint
	std::vector<std::pair<int, Vector2>> extrusionPoints;
	for (int i = 0; i < extrusions; ++i) {
		// Select random side (0=bottom, 1=right, 2=top, 3=left)
		int side = Random::Range(0, 3);

		// Get side vertices
		Vector2 start = footprint[side];
		Vector2 end = footprint[(side + 1) % 4];

		// Split point along side (20-80% from start)
		float split = Random::Range(0.2f, 0.8f);
		Vector2 midpoint = start + split * (end - start);

		// Extrusion parameters
		float extrusionLength = radius * Random::Range(0.1f, 0.3f);
		Vector2 direction;

		// Determine extrusion direction based on side
		switch (side) {
			case 0: direction = { 0, -1 }; break; // Bottom side extrudes down
			case 1: direction = { 1, 0 }; break;  // Right side extrudes right
			case 2: direction = { 0, 1 }; break;  // Top side extrudes up
			case 3: direction = { -1, 0 }; break; // Left side extrudes left
		}

		// Create extrusion point
		Vector2 extrusionPoint = midpoint + direction * extrusionLength;

		// Store the side index and the new vertices
		extrusionPoints.push_back({ side, midpoint });
		extrusionPoints.push_back({ side, extrusionPoint });
	}

	// Sort extrusion points by side index in descending order
	// This ensures we insert vertices in the correct order without invalidating indices
	std::sort(extrusionPoints.begin(), extrusionPoints.end(), [](const auto& a, const auto& b) {
		return a.first > b.first;
	});

	// Insert extrusion points into the footprint
	for (const auto& point : extrusionPoints) {
		int side = point.first;
		Vector2 vertex = point.second;

		// Insert the vertex after the side's start point
		footprint.insert(footprint.begin() + side + 1, vertex);
	}

	return footprint;
}

// Function to generate a building mesh from the polygon
void BuildingGenerator::GenerateBuildingMesh(const std::vector<Vector2> &polygon, float height)
{
	auto &vertices = mesh->GetVertices();
	auto &indices = mesh->GetIndices();

	vertices.clear();
	indices.clear();

	mesh->Generate();

	int numVertices = polygon.size();
	int baseIndex = 0;

	// Add vertices for the base (floor) and top (roof)
	for (int i = 0; i < numVertices; ++i)
	{
		// Base vertices (y = 0)
		vertices.push_back({{polygon[i].x, 0.0f, polygon[i].y}, {0, -1, 0}, {0, 0}});
		// Top vertices (y = height)
		vertices.push_back({{polygon[i].x, height, polygon[i].y}, {0, 1, 0}, {1, 1}});
	}

	// Add side faces indices
	for (int i = 0; i < numVertices; ++i)
	{
		int next = (i + 1) % numVertices;

		// Base (floor) faces
		indices.push_back(baseIndex + i * 2);	   // Bottom left
		indices.push_back(baseIndex + i * 2 + 1); // Top left
		indices.push_back(baseIndex + next * 2);  // Bottom right

		indices.push_back(baseIndex + next * 2);  // Bottom right
		indices.push_back(baseIndex + i * 2 + 1); // Top left
		indices.push_back(baseIndex + next * 2 + 1); // Top right
	}

    int roofCenterIndex = vertices.size();
    vertices.push_back({{0.0f, height, 0.0f}, {0, 1, 0}, {0, 0}});  // Roof center vertex

    for (int i = 0; i < numVertices; ++i) {
        // Create a face connecting the roof center to the top vertices
        int next = (i + 1) % numVertices;
        indices.push_back(roofCenterIndex);  // Roof center
        indices.push_back(baseIndex + next * 2 + 1); // Top right
        indices.push_back(baseIndex + i * 2 + 1); // Top left
    }

	mesh->RecalculateNormals();
	mesh->Generate();
}

void BuildingGenerator::CreateTerrain()
{
	auto terrainObject = GameObject::CreatePrimitive(PrimitiveType::Terrain);
	auto terrain = terrainObject->GetComponent<Terrain>();	
	auto material = terrain->GetMaterial();

    float posX = -(terrain->GetWidth() * terrain->GetScale() * 0.5);
    float posZ = (terrain->GetDepth() * terrain->GetScale() * 0.5);
    terrainObject->GetTransform()->SetPosition(Vector3(posX, 0, posZ));
	
	// Noise::SetFrequency(0.015);

	// for(uint32_t y = 0; y < terrain->GetDepth() + 1; y++)
	// {
	// 	for(uint32_t x = 0; x < terrain->GetWidth() + 1; x++)
	// 	{
	// 		float height = Noise::GetSample(x, y) * 2.0;
	// 		height -= 1.0f;
	// 		terrain->SetHeight(x, y, height * 25.0f);
	// 	}
	// }

	// terrain->Update();

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

Texture2D *BuildingGenerator::LoadTexture(const std::string &filepath)
{
	Image image(filepath);
	if(image.IsLoaded())
	{
		return Resources::AddTexture2D(filepath, Texture2D(&image));
	}
	Debug::WriteLine("Failed " + filepath);
	return nullptr;
}