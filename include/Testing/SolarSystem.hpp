#ifndef SOLARSYSTEM_HPP
#define SOLARSYSTEM_HPP

#include "../GFX.hpp"

struct Planet
{
    GameObject *gameObject;
    std::string name;
    float radius;
    float mass;
    float distanceFromSun;
    Vector3 velocity;
    Color color;
};

class SolarSystem : public GameBehaviour
{
private:
    std::vector<Planet> planets;
    const float distanceScale = 1e-9f; // Scaled down distance between planets and the sun
    const float sizeScale = 1e-5f; // Scaled down radius of planets
    const float gravityConstant = 6.67430e-11f; // Gravitational constant in SI units
protected:
    // Start is called before the first frame update
    void OnInitialize() override;
    void OnUpdate() override;
    void OnFixedUpdate() override;
    void CreatePlanet(const std::string &name, float radius, float mass, float distanceFromSun, const Color &color);
    void OnResourceBatchLoadedAsync(const ResourceBatch &resourceBatch) override;
    void OnLoadingCompleted();
    Texture2D *LoadTexture(const std::string &filepath);
};

#endif