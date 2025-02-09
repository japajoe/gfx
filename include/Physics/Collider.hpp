#ifndef GFX_COLLIDER_HPP
#define GFX_COLLIDER_HPP

#include "../Core/Component.hpp"

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
	public:
        Collider();
        virtual ~Collider();
		ColliderType GetType() const;
	};
}

#endif