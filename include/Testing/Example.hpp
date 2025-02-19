#ifndef EXAMPLE_HPP
#define EXAMPLE_HPP

#include "../GFX.hpp"

class Example : public GameBehaviour
{
private:
	std::vector<Rigidbody*> cubes;
	Rigidbody* ball = nullptr;
	ProceduralSkybox2Material *skyboxMaterial = nullptr;
	void ResetCubes();
	void ShootBall();
protected:
	void OnInitialize() override;
	void OnUpdate() override;
	void OnGUI() override;
};

#endif