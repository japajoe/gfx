#ifndef GFX_SPHERECOLLIDER_HPP
#define GFX_SPHERECOLLIDER_HPP

#include "Collider.hpp"
#include "../../System/Numerics/Vector3.hpp"

namespace GFX
{
	class SphereCollider : public Collider
	{
	private:
		float radius;
	public:
	    SphereCollider();
	    void SetRadius(float radius);
	    float GetRadius() const;
		void RenderShape() override;
	};
}

#endif