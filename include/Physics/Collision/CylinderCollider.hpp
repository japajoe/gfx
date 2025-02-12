#ifndef GFX_CYLINDERCOLLIDER_HPP
#define GFX_CYLINDERCOLLIDER_HPP

#include "Collider.hpp"
#include "../../System/Numerics/Vector3.hpp"

namespace GFX
{
	class CylinderCollider : public Collider
	{
	private:
		float height;
		float radius;
	public:
	    CylinderCollider();
	    void SetHeight(float height);
	    float GetHeight() const;
	    void SetRadius(float radius);
	    float GetRadius() const;
	};
}

#endif