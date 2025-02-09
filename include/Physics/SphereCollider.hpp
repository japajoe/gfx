#ifndef GFX_SPHERECOLLIDER_HPP
#define GFX_SPHERECOLLIDER_HPP

#include "Collider.hpp"
#include "../System/Numerics/Vector3.hpp"

namespace GFX
{
	class SphereCollider : public Collider
	{
	private:
		Vector3 center;
		float radius;
	public:
	    SphereCollider();
	    void SetCenter(const Vector3 &center);
	    Vector3 GetCenter() const;
	    void SetRadius(float radius);
	    float GetRadius() const;
	};
}

#endif