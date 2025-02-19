#ifndef EXAMPLE_HPP
#define EXAMPLE_HPP

#include "../GFX.hpp"
#include "Car.hpp"

// class Wheely : public GameBehaviour
// {
// private:
//     Rigidbody *rb;
//     float minLength;
//     float maxLength;
//     float lastLength;
//     float springLength;
//     float springVelocity;
// 	float springForce;
// 	float damperForce;
// 	Vector3 suspensionForce;
// 	bool isGrounded;
// 	Vector3 GetWheelOffset() const;
// 	void DrawDebugLines();
// 	void DrawCircle(const Vector3 &center, float radius, const Quaternion &rotation);
// protected:
// 	void OnInitialize() override;
// 	void OnUpdate() override;
// 	void OnFixedUpdate() override;
// public:
// 	float restLength;
//     float springTravel;
//     float springStiffness;
//     float damperStiffness;
//     float wheelRadius;
// 	float motorTorque;
// 	float steerAngle;
// 	Vector3 center;
// };

class Example : public GameBehaviour
{
// private:
// 	Rigidbody* rigidbody = nullptr;
// 	std::vector<Wheely*> wheels;
// 	void CreateVehicle();
protected:
	void OnInitialize() override;
	void OnUpdate() override;
	void OnFixedUpdate() override;
	void OnGUI() override;
};

#endif