#ifndef CHARACTERDEMO_HPP
#define CHARACTERDEMO_HPP

#include "GFX.hpp"

class CharacterDemo : public GameBehaviour
{
private:
	Rigidbody *ball = nullptr;
	GameObject *player;
	float horizontal = 0.0f;
	float vertical = 0.0f;
	bool isJumping = false;
	float verticalVelocity = 0.0f;
	Vector3 movementDirection;
	BoundingBox bounds;
	float speed = 10.0f;
	float jumpForce = 2.0f;
	bool showBounds = false;
	int maxBounces = 5;
	float skinWidth = 0.015f;
	float maxSlopeAngle = 15.0f;
	void GetInput();
	void Rotate();
	void Move();
	void DrawBounds();
	Vector3 CollideAndSlide(const Vector3 &position, const Vector3 &velocity, const Vector3 &initialVelocity, bool gravityPass, bool isGrounded, int depth);
protected:
	void OnInitialize() override;
	void OnUpdate() override;
	void OnLateUpdate() override;
	void OnGUI() override;
};

#endif