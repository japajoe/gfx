#ifndef VEHICLE_HPP
#define VEHICLE_HPP

#include "../GFX.hpp"

struct WheelInfo
{
	Transform *visual;
	WheelCollider *collider;
};

struct VehicleSettings
{
	float mass;
	float turnRadius;
	float antiRoll;
	float maxSpeed;
	WheelInfo frontLeft;
	WheelInfo frontRight;
	WheelInfo rearLeft;
	WheelInfo rearRight;
	Vector3 colliderPosition;
	Vector3 colliderSize;
};

struct AntiRollBar
{
	Rigidbody *body;
	WheelCollider *left;
	WheelCollider *right;
	float antiRoll;
	void Update();
};

class Vehicle : public GameBehaviour
{
private:
	Rigidbody *rigidbody;
	AntiRollBar antiRollBar;
	float mass;
	float wheelBase;
	float rearTrack;
	float turnRadius;
	std::vector<WheelInfo> wheels;
	Oscillator *oscillator = nullptr;
protected:
	void OnInitialize() override;
	void OnUpdate() override;
	void OnFixedUpdate() override;
public:
	void Configure(const VehicleSettings &settings);
	void Flip();
	float GetSpeed() const;
};

#endif