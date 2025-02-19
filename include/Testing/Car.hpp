#ifndef CAR_HPP
#define CAR_HPP

#include "../GFX.hpp"

class Car : public GameBehaviour
{
private:
	Rigidbody *rigidbody;
	std::vector<WheelCollider*> wheels;
	float angleLeft;
	float angleRight;
protected:
	void OnInitialize() override;
	void OnUpdate() override;
	void OnFixedUpdate() override;
	void OnGUI() override;
public:
	float wheelBase;
	float rearTrack;
	float turnRadius;
};

#endif