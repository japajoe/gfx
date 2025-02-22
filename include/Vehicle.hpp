#ifndef VEHICLE_HPP
#define VEHICLE_HPP

#include "GFX.hpp"

struct WheelInfo
{
	Transform *visual;
	WheelCollider *collider;
};

struct VehicleSettings
{
	float mass;
	float turnRadius;
	WheelInfo frontLeft;
	WheelInfo frontRight;
	WheelInfo rearLeft;
	WheelInfo rearRight;
	Vector3 colliderPosition;
	Vector3 colliderSize;
};

class Vehicle : public GameBehaviour
{
private:
	Rigidbody *rigidbody;
	float mass;
	float wheelBase;
	float rearTrack;
	float turnRadius;
	std::vector<WheelInfo> wheels;
protected:
	void OnInitialize() override;
	void OnUpdate() override;
public:
	void Configure(const VehicleSettings &settings);
};

#endif