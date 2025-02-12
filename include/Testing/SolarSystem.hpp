#ifndef SOLARSYSTEM_HPP
#define SOLARSYSTEM_HPP

#include "../GFX.hpp"

struct Planet
{
	GameObject *object;
	Transform *transform;
    Rigidbody *body;
	float mass;
	float radius;
	float distanceFromSun;
    Color color;
    std::string name;
};

class SolarSystem : public GameBehaviour
{
private:
    //real value of gravitational constant is 6.67408 × 10-11
    //can increase to make thing go faster instead of increase timestep of Unity
    const float G = 1000.0f;
    //const float G = 6.674f;
    std::vector<Planet> celestials;

    bool IsElipticalOrbit = false;
protected:
    // Start is called before the first frame update
    void OnInitialize() override
    {
        Camera::GetMain()->SetClearColor(Color::Black());
        Camera::GetMain()->SetFarClippingPlane(1000000);
        auto fps = Camera::GetMain()->GetGameObject()->AddComponent<FirstPersonCamera>();
        fps->SetSpeed(fps->GetSpeed() * 1000);

        World::SetFogEnabled(false);

        CreatePlanet("Sun",     333000,     696340, 0, Color::Orange());
        CreatePlanet("Mercury", 0.055,      2440,   63.81, Color::Gray());
        CreatePlanet("Venus",   0.815,      6052,   107.59, Color::Yellow());
        CreatePlanet("Earth",   1.000,      6371,   151.48, Color::Blue());
        CreatePlanet("Mars",    0.107,      3390,   248.84, Color::Red());
        CreatePlanet("Jupiter", 317.800,    69911,  755.91, Color::Brown());
        CreatePlanet("Saturn",  95.160,     58232,  1487.80, Color::Gold());
        CreatePlanet("Uranus",  14.540,     25362,  2954.60, Color::Cyan());
        CreatePlanet("Neptune", 17.150,     24622,  4475.50, Color::Blue());

        SetInitialVelocity();
    }

    // Update is called once per frame
    void OnFixedUpdate() override
    {
        Gravity();

        auto camera = Camera::GetMain();

        GUI::BeginFrame();

        for(auto obj : celestials)
        {
            Vector3 pos;
            
            if(camera->WorldToScreenPoint(obj.transform->GetPosition(), pos))
            {
                GUI::Text(Rectangle(pos.x, pos.y, 100, 20), obj.name);
            }
        }

        GUI::EndFrame();
    }

    void SetInitialVelocity()
    {
        for (auto a : celestials)
        {
            for (auto b : celestials)
            {
                if(a.object != b.object)
                {
                    float m2 = b.mass;
                    float r = Vector3f::Distance(a.transform->GetPosition(), b.transform->GetPosition());

                    a.transform->LookAt(b.transform);

                    if (IsElipticalOrbit)
                    {
                        // Eliptic orbit = G * M  ( 2 / r + 1 / a) where G is the gravitational constant, M is the mass of the central object, r is the distance between the two bodies
                        // and a is the length of the semi major axis (!!! NOT GAMEOBJECT a !!!)
                        auto velocity = a.transform->GetRight() * glm::sqrt((G * m2) * ((2 / r) - (1 / (r * 1.5f))));
                        a.body->SetLinearVelocity(velocity);
                    }
                    else
                    {
                        //Circular Orbit = ((G * M) / r)^0.5, where G = gravitational constant, M is the mass of the central object and r is the distance between the two objects
                        //We ignore the mass of the orbiting object when the orbiting object's mass is negligible, like the mass of the earth vs. mass of the sun
                        auto velocity = a.transform->GetRight() * glm::sqrt((G * m2) / r);
                        a.body->SetLinearVelocity(velocity);
                    }
                }
            }
        }
    }

    void Gravity()
    {
        for (auto a : celestials)
        {
            for (auto b : celestials)
            {
                if (a.name != "Sun") // Sun doesn't apply forces to itself
                {
                    if (a.object != b.object)
                    {
                        float m1 = a.mass;
                        float m2 = b.mass;
                        float r = Vector3f::Distance(a.transform->GetPosition(), b.transform->GetPosition());

                        Vector3 direction = Vector3f::Normalize(b.transform->GetPosition() - a.transform->GetPosition());

                        a.body->AddForce(direction * (G * (m1 * m2) / (r * r)));
                    }
                }
            }
        }
    }

    void CreatePlanet(const std::string &name, float mass, float radius, float distanceFromSun, const Color &color)
    {
        const float earthMass = 1.0f;

        Planet planet;
        planet.radius = radius * 0.001f;
        planet.distanceFromSun = distanceFromSun * 100;
        planet.mass = mass * earthMass;
        planet.name = name;
        planet.color = color;
        
        planet.object = GameObject::CreatePrimitive(PrimitiveType::Sphere);
        auto material = planet.object->GetComponent<MeshRenderer>()->GetMaterial<DiffuseMaterial>(0);
        material->SetDiffuseColor(planet.color);
        material->SetReceiveShadows(false);

        planet.transform = planet.object->GetTransform();
        planet.transform->SetLocalScale(Vector3(planet.radius * 2, planet.radius * 2, planet.radius * 2));

        if(planet.distanceFromSun > 0)
        {
            float randomAngle = Random::Range(0.0f, 360.0f);  // Random angle between 0 and 360 degrees
            float angleInRadians = glm::radians(randomAngle);
            float x = planet.distanceFromSun * cos(angleInRadians);
            float z = planet.distanceFromSun * sin(angleInRadians); 
            planet.transform->SetPosition(Vector3(x, 0, z));
        }
        
        auto collider = planet.object->AddComponent<SphereCollider>();
        collider->SetRadius(planet.radius);
        planet.body = planet.object->AddComponent<Rigidbody>(planet.mass);
        planet.body->SetGravityFactor(0.0f);
        celestials.push_back(planet);
    }
};

#endif