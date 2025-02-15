#include "SolarSystem.hpp"

void SolarSystem::OnInitialize()
{
	Camera::GetMain()->SetClearColor(Color::Black());
	Camera::GetMain()->SetFarClippingPlane(1000000);
	auto fps = Camera::GetMain()->GetGameObject()->AddComponent<FirstPersonCamera>();
	fps->SetSpeed(fps->GetSpeed() * 1000.0f);

	World::SetFogEnabled(false);

	std::vector<std::string> resources = {
		"../res/Textures/Planets/Sun.jpg",
		"../res/Textures/Planets/Mercury.jpg",
		"../res/Textures/Planets/Venus.jpg",
		"../res/Textures/Planets/Earth.jpg",
		"../res/Textures/Planets/Mars.jpg",
		"../res/Textures/Planets/Jupiter.jpg",
		"../res/Textures/Planets/Saturn.jpg",
		"../res/Textures/Planets/Uranus.jpg",
		"../res/Textures/Planets/Neptune.jpg"
	};

	Resources::LoadAsyncBatchFromFile(ResourceType::Texture2D, resources);
}

void SolarSystem::OnUpdate()
{
	if(Input::GetKeyDown(KeyCode::Escape))
		Application::Quit();

	if(Input::GetKeyDown(KeyCode::C))
		Input::SetMouseCursor(!Input::IsCursorVisible());

	GUI::BeginFrame();

	if(planets.size() > 0)
	{
		auto camera = Camera::GetMain();

		for(auto obj : planets)
		{
			Vector3 pos;
			
			if(camera->WorldToScreenPoint(obj.gameObject->GetTransform()->GetPosition(), pos))
			{
				GUI::Text(Rectangle(pos.x, pos.y, 100, 20), obj.name);
			}
		}
	}
	else
	{
		GUI::Text(Rectangle(10, 10, 100, 20), "Loading " + std::to_string(Time::GetTime()));
	}

	GUI::EndFrame();

	const float sunMass = 1.989e30f;
	const Vector3 sunPosition(0 ,0, 0);

	for (size_t i = 0; i < planets.size(); i++)
	{
		Planet &planet = planets[i];

		if(planet.name == "Sun")
			continue;

		// Calculate the gravitational force between the planet and the sun
		float distance = planet.distanceFromSun;
		float forceMagnitude = (gravityConstant * sunMass * planet.mass) / (distance * distance);

		// Calculate acceleration towards the sun
		float acceleration = forceMagnitude / planet.mass;

		// The velocity direction will always be perpendicular to the radius vector
		// Using simple physics, we can calculate the change in velocity and position
		Vector3 directionToSun = Vector3f::Normalize(sunPosition - planet.gameObject->GetTransform()->GetPosition());
		Vector3 tangentialVelocity = Vector3f::Perpendicular(planet.velocity);  // This gives an initial tangential velocity for orbit

		// Update velocity based on acceleration (simplified for a circular orbit)
		planet.velocity += directionToSun * acceleration * Time::GetDeltaTime();

		// Update position based on velocity
		Vector3 position = planet.gameObject->GetTransform()->GetPosition() + planet.velocity * Time::GetDeltaTime();

		planet.gameObject->GetTransform()->SetPosition(position);
	}


	// for (size_t i = 0; i < planets.size(); i++)
	// {
	// 	auto &planet = planets[i];

	// 	if(planet.name == "Sun")
	// 		continue;

	// 	Vector3 directionToSun = -planets[i].gameObject->GetTransform()->GetPosition();
	// 	float distanceToSun = glm::length(directionToSun);

	// 	// Apply gravity using Newton's law of gravitation
	// 	float gravitationalForce = gravityConstant * planet.mass / glm::pow(distanceToSun, 2);

	// 	// Calculate the acceleration based on gravity
	// 	Vector3 gravityAcceleration = Vector3f::Normalize(directionToSun) * gravitationalForce / planet.mass;

	// 	planet.velocity += gravityAcceleration * Time::GetDeltaTime();

	// 	// Update the planet's position (simple orbital movement, not considering other forces for now)
	// 	//auto position = planet.gameObject->GetTransform()->GetPosition() + gravityAcceleration * Time::GetDeltaTime();
	// 	//auto position = planet.gameObject->GetTransform()->GetPosition() + planet.velocity * Time::GetDeltaTime();
		
	// 	//planet.gameObject->GetTransform()->SetPosition(position);
	// }
}

// Update is called once per frame
void SolarSystem::OnFixedUpdate()
{

}

void SolarSystem::CreatePlanet(const std::string &name, float radius, float mass, float distanceFromSun, const Color &color)
{
	Planet planet;
	planet.radius = radius;
	planet.mass = mass;
	planet.distanceFromSun = distanceFromSun;
	planet.color = color;
	planet.name = name;

	float scaledRadius = radius * 1e-3f;
	float scaledDistance = distanceFromSun * 1e-4f;

	if(name == "Sun")
	{
		scaledRadius = radius * 1e-3f;
		scaledDistance = 0;
		planet.velocity = Vector3(0, 0, 0);
	}
	else
	{
		// For each planet, set an initial tangential velocity based on its orbital radius and orbital period.
		float orbitalVelocity = sqrt(gravityConstant * 1.989e30f / planet.distanceFromSun); // Simplified for circular orbits

		// Initialize velocity when creating the planet:
		planet.velocity = Vector3(orbitalVelocity, 0, 0); // Tangential velocity
	}

	float angularOffset = (name == "Sun") ? 0.0f : Random::Range(0.0f, 2.0f * glm::pi<float>());
	float xPosition = scaledDistance * glm::cos(angularOffset);
	float zPosition = scaledDistance * glm::sin(angularOffset);

	// Create planet as a sphere primitive
	planet.gameObject = GameObject::CreatePrimitive(PrimitiveType::Sphere);
	planet.gameObject->GetTransform()->SetLocalScale(Vector3(scaledRadius, scaledRadius, scaledRadius));
	planet.gameObject->GetTransform()->SetPosition(Vector3(xPosition, 0, zPosition));

	// Set the color of the planet
	auto planetRenderer = planet.gameObject->GetComponent<MeshRenderer>();

	Texture2D *texture = Resources::FindTexture2D("../res/Textures/Planets/" + name + ".jpg");

	if(texture)
		planetRenderer->GetMaterial<DiffuseMaterial>(0)->SetDiffuseTexture(texture);
	else
		planetRenderer->GetMaterial<DiffuseMaterial>(0)->SetDiffuseColor(color);

	planets.push_back(planet);
}

void SolarSystem::OnResourceBatchLoadedAsync(const ResourceBatch &resourceBatch)
{
	for(auto &resource : resourceBatch.resources)
	{
		Resources::AddTexture2D(resource.name, Texture2D(resource.data.data(), resource.data.size(), resource.width, resource.height, resource.channels));
	}

	OnLoadingCompleted();
}

void SolarSystem::OnLoadingCompleted()
{
	CreatePlanet("Sun", 696340, 1.989e30f, 0.0f, Color::Yellow());
	CreatePlanet("Mercury", 2439.7f, 3.3011e23f, 57910000, Color(0.5f, 0.5f, 0.5f));
	CreatePlanet("Venus", 6051.8f, 4.8675e24f, 108200000, Color(1.0f, 0.8f, 0.0f));
	CreatePlanet("Earth", 6371, 5.97237e24f, 149600000, Color::Blue());
	CreatePlanet("Mars", 3389.5f, 6.4171e23f, 227900000, Color::Red());
	CreatePlanet("Jupiter", 69911, 1.8982e27f, 778500000, Color(0.8f, 0.5f, 0.0f));
	CreatePlanet("Saturn", 58232, 5.6834e26f, 1433500000, Color(1.0f, 1.0f, 0.4f));
	CreatePlanet("Uranus", 25362, 8.6810e25f, 2872500000, Color(0.4f, 0.8f, 1.0f));
	CreatePlanet("Neptune", 24622, 1.02413e26f, 4495100000, Color(0.2f, 0.2f, 1.0f));
}

Texture2D *SolarSystem::LoadTexture(const std::string &filepath)
{
	Image image(filepath);
	if(image.IsLoaded())
	{
		return Resources::AddTexture2D(filepath, Texture2D(&image));
	}
	Debug::WriteLine("Failed " + filepath);
	return nullptr;
}

