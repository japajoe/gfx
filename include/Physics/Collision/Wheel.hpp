#ifndef GFX_WHEEL_HPP
#define GFX_WHEEL_HPP

#include "../../Core/GameBehaviour.hpp"
#include "../../System/Numerics/Vector2.hpp"
#include "../../System/Numerics/Vector3.hpp"
#include "../../System/Numerics/Quaternion.hpp"

namespace GFX
{
	class Rigidbody;
	class Transform;

	class Wheel : public GameBehaviour
	{
	private:
		Transform *transform;
		Rigidbody *rigidBody;
		Vector3 center;
		float springStrength;
		float springDamper;
		float springTravel;
		float restLength;
		float minLength;
		float maxLength;
		float radius;
		void ApplySpringForce(float rayDistance);
		Vector3 GetWheelOffset() const;
		void SetMinMaxLength();
	protected:
		void OnInitialize() override;
		void OnFixedUpdate() override;
	public:
		Wheel();
		void SetCenter(const Vector3 &center);
		Vector3 GetCenter() const;
	};
}

#endif