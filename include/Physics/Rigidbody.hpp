#ifndef GFX_RIGIDBODY_HPP
#define GFX_RIGIDBODY_HPP

#include "../Core/Component.hpp"
#include "../System/Numerics/Quaternion.hpp"
#include "../System/Numerics/Vector3.hpp"
#include <memory>

namespace JPH
{
	class Body;
}

namespace GFX
{
	enum class ForceMode
	{
		Force,
		Impulse
	};

	struct PhysicsBody;

	class Rigidbody : public Component
	{
	friend class Physics;
	private:
		PhysicsBody *body;
		float mass;
		bool CreateShape();
		bool Initialize();
    protected:
        void OnInitialize() override;
        void OnDestroy() override;
        void OnActivate() override;
        void OnDeactivate() override;
		JPH::Body *GetBody();
	public:
		Rigidbody();
		Rigidbody(float mass);
		float GetMass() const;
		void SetMass(float value);
		void AddForce(const Vector3 &force, ForceMode mode = ForceMode::Force);
		void AddForceAtPoint(const Vector3 &force, const Vector3 &point, ForceMode mode = ForceMode::Force);
		void AddRelativeForce(const Vector3 &force, ForceMode mode = ForceMode::Force);
		void AddTorque(const Vector3 &torque);
		void AddRelativeTorque(const Vector3 &torque);
		void SetLinearVelocity(const Vector3 &velocity);
		void SetAngularVelocity(const Vector3 &velocity);
		void MovePosition(const Vector3 &position);
		void MoveRotation(const Quaternion &rotation);
	};
}

#endif