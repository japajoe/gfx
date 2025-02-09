#include "Collider.hpp"

namespace GFX
{
	Collider::Collider() : Component()
	{
		type = ColliderType::Box;
	}
	
	Collider::~Collider()
	{

	}

	ColliderType Collider::GetType() const
	{
		return type;
	}
}