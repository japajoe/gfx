#include "SphereCollider.hpp"

namespace GFX
{
    SphereCollider::SphereCollider() : Collider()
    {
        this->type = ColliderType::Sphere;
        this->radius = 0.5f;
    }

    void SphereCollider::SetRadius(float radius)
    {
        this->radius = radius;
    }

    float SphereCollider::GetRadius() const
    {
        return radius;
    }
}