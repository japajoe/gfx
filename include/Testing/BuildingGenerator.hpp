#include "../GFX.hpp"

class BuildingGenerator : public GameBehaviour
{
private:
	GameObject *building = nullptr;
	MeshRenderer *renderer = nullptr;
	Mesh *mesh = nullptr;
	void Generate();
	std::vector<Vector2> GenerateRandomPolygon(int sides, float radius);
	void GenerateBuildingMesh(const std::vector<Vector2> &polygon, float height);
protected:
	void OnInitialize() override;
	void OnUpdate() override;
};