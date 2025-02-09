#ifndef GFX_RIGIDBODY_HPP
#define GFX_RIGIDBODY_HPP

#include "../../Core/Component.hpp"
#include <memory>

namespace JPH
{
	class Body;
}

namespace GFX
{
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
	};
}

#endif