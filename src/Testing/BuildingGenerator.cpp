#include "BuildingGenerator.hpp"

void BuildingGenerator::OnInitialize()
{
	auto camera = Camera::GetMain();
	camera->GetGameObject()->AddComponent<FirstPersonCamera>();

	GameObject::CreatePrimitive(PrimitiveType::ProceduralSkybox);

	building = GameObject::CreatePrimitive(PrimitiveType::Plane);
	renderer = building->GetComponent<MeshRenderer>();
	auto material = renderer->GetMaterial<DiffuseMaterial>(0);
	material->SetDiffuseColor(Color::SkyBlue());
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

void BuildingGenerator::Generate()
{
	int sides = Random::Range(3, 8);
	float radius = Random::Range(15, 50);
	float height = 3 + (glm::round(Random::Range(0, 12) * 3));
	auto polygon = GenerateRandomPolygon(sides, radius);
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
		vertices.push_back({{polygon[i].x, height, polygon[i].y}, {0, 1, 0}, {0, 0}});
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