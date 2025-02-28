#ifndef PHYSICSDEMO_HPP
#define PHYSICSDEMO_HPP

#include "GFX.hpp"

class PhysicsDemo : public GameBehaviour
{
private:
	std::vector<Rigidbody*> cubes;
	int numCubesX = 10;
	int numCubesY = 10;
	Rigidbody *ball = nullptr;
	Font *font = nullptr;
	void CreateGround();
	void CreateBall();
	void CreateCubes();
	void ResetCubes();
protected:
	void OnInitialize() override;
	void OnUpdate() override;
	void OnGUI() override;
};

#endif