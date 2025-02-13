#include "../GFX.hpp"

class BuildingGenerator : public GameBehaviour
{
private:
	GameObject *building = nullptr;
	MeshRenderer *renderer = nullptr;
	Mesh *mesh = nullptr;
	void Generate();
	std::vector<Vector2> GenerateRandomPolygon(int sides, float radius);
	std::vector<Vector2> GenerateRandomPolygon2(int sides, float radius);
	void GenerateBuildingMesh(const std::vector<Vector2> &polygon, float height);
	void CreateTerrain();
	Texture2D *LoadTexture(const std::string &filepath);
protected:
	void OnInitialize() override;
	void OnUpdate() override;
};