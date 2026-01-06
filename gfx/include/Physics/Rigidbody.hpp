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

	enum class CollisionDetectionMode : uint8_t
	{
		Discrete,
		Linear,
	};

	enum class RigidbodyConstraints : uint8_t
	{
		None				= 0b000000,									///< No degrees of freedom are allowed. Note that this is not valid and will crash. Use a static body instead.
		All					= 0b111111,									///< All degrees of freedom are allowed
		TranslationX		= 0b000001,									///< Body can move in world space X axis
		TranslationY		= 0b000010,									///< Body can move in world space Y axis
		TranslationZ		= 0b000100,									///< Body can move in world space Z axis
		RotationX			= 0b001000,									///< Body can rotate around world space X axis
		RotationY			= 0b010000,									///< Body can rotate around world space Y axis
		RotationZ			= 0b100000,									///< Body can rotate around world space Z axis
		Plane2D				= TranslationX | TranslationY | RotationZ,	///< Body can only move in X and Y axis and rotate around Z axis
	};

	struct RigidbodySettings
	{
		float mass;
		RigidbodyConstraints constraints;
	};

	struct PhysicsBody;

	class Rigidbody : public Component
	{
	friend class Physics;
	private:
		std::unique_ptr<PhysicsBody> body;
		float mass;
		bool isActive;
		RigidbodyConstraints constraints;
		bool CreateShape();
		bool Initialize();
		bool IsInitialized() const;
    protected:
        void OnInitialize() override;
        void OnDestroy() override;
        void OnActivate() override;
        void OnDeactivate() override;
		JPH::Body *GetBody();
	public:
		Rigidbody();
		Rigidbody(float mass);
		Rigidbody(const RigidbodySettings &settings);
		~Rigidbody();
		float GetMass() const;
		void SetMass(float value);
		void AddForce(const Vector3 &force, ForceMode mode = ForceMode::Force);
		void AddForceAtPoint(const Vector3 &force, const Vector3 &point, ForceMode mode = ForceMode::Force);
		void AddRelativeForce(const Vector3 &force, ForceMode mode = ForceMode::Force);
		void AddTorque(const Vector3 &torque);
		void AddRelativeTorque(const Vector3 &torque);
		void SetLinearVelocity(const Vector3 &velocity);
		Vector3 GetLinearVelocity() const;
		void SetAngularVelocity(const Vector3 &velocity);
		void MovePosition(const Vector3 &position);
		void MoveRotation(const Quaternion &rotation);
		Vector3 GetPointVelocity(const Vector3 &relativePosition);
		Vector3 GetCenterOfMass();
		void SetBounciness(float bounciness);
		float GetBounciness() const;
		void SetLinearDrag(float drag);
		float GetLinearDrag() const;
		void SetAngularDrag(float drag);
		float GetAngularDrag() const;
		void SetCollisionDetectionMode(CollisionDetectionMode mode);
		CollisionDetectionMode GetCollisionDetectionMode() const;
		void SetGravityFactor(float factor);
		float GetGravityFactor() const;
		bool IsSleeping() const;
		RigidbodyConstraints GetConstraints() const;
	};
}

#endif