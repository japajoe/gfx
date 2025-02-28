#ifndef GFX_CAPSULECOLLIDER_HPP
#define GFX_CAPSULECOLLIDER_HPP

#include "Collider.hpp"
#include "../../System/Numerics/Vector3.hpp"

namespace GFX
{
	class CapsuleCollider : public Collider
	{
	private:
		float height;
		float radius;
	public:
	    CapsuleCollider();
	    void SetHeight(float height);
	    float GetHeight() const;
	    void SetRadius(float radius);
	    float GetRadius() const;
	};
}

#endif