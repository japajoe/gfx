#include "Collider.hpp"

namespace GFX
{
	Collider::Collider() : Component()
	{
		type = ColliderType::Box;
		center = Vector3(0, 0, 0);
	}
	
	Collider::~Collider()
	{

	}

	ColliderType Collider::GetType() const
	{
		return type;
	}

    void Collider::SetCenter(const Vector3 &center)
    {
        this->center = center;
    }

    Vector3 Collider::GetCenter() const
    {
        return center;
    }

	void Collider::RenderShape()
	{

	}
}