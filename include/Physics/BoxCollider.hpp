#ifndef GFX_BOXCOLLIDER_HPP
#define GFX_BOXCOLLIDER_HPP

#include "Collider.hpp"
#include "../System/Numerics/Vector3.hpp"

namespace GFX
{
	class BoxCollider : public Collider
	{
	private:
		Vector3 center;
		Vector3 size;
	public:
	    BoxCollider();
	    void SetCenter(const Vector3 &center);
	    Vector3 GetCenter() const;
	    void SetSize(const Vector3 &size);
	    Vector3 GetSize() const;
	};
}

#endif