#include "BoxCollider.hpp"

namespace GFX
{
    BoxCollider::BoxCollider() : Collider()
    {
        this->type = ColliderType::Box;
        this->center = Vector3(0, 0, 0);
        this->size = Vector3(1, 1, 1);
    }

    void BoxCollider::SetCenter(const Vector3 &center)
    {
        this->center = center;
    }

    Vector3 BoxCollider::GetCenter() const
    {
        return center;
    }

    void BoxCollider::SetSize(const Vector3 &size)
    {
        this->size = size;
    }

    Vector3 BoxCollider::GetSize() const
    {
        return size;
    }
}