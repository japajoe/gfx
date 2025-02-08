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
		std::unique_ptr<PhysicsBody> body;
    protected:
        void OnInitialize() override;
        void OnDestroy() override;
        void OnActivate() override;
        void OnDeactivate() override;
	public:
		JPH::Body *GetBody();
	};
}

#endif