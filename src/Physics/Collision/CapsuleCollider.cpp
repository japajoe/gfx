#include "CapsuleCollider.hpp"

namespace GFX
{
    CapsuleCollider::CapsuleCollider() : Collider()
    {
        this->type = ColliderType::Capsule;
        this->height = 1.0f;
        this->radius = 0.5f;
    }

    void CapsuleCollider::SetHeight(float height)
    {
        this->height = height;
    }

    float CapsuleCollider::GetHeight() const
    {
        return height;
    }

    void CapsuleCollider::SetRadius(float radius)
    {
        this->radius = radius;
    }

    float CapsuleCollider::GetRadius() const
    {
        return radius;
    }
}