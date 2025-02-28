#ifndef GFX_COLLIDER_HPP
#define GFX_COLLIDER_HPP

#include "../../Core/Component.hpp"
#include "../../System/Numerics/Vector3.hpp"

namespace GFX
{
    enum class ColliderType
    {
        Box,
        Capsule,
        Cylinder,
        Mesh,
        Sphere,
        Terrain
    };

	class Collider : public Component
	{
	protected:
		ColliderType type;
        Vector3 center;
	public:
        Collider();
        virtual ~Collider();
		ColliderType GetType() const;
	    void SetCenter(const Vector3 &center);
	    Vector3 GetCenter() const;
        virtual void RenderShape();
	};
}

#endif