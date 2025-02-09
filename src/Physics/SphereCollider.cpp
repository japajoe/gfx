#include "SphereCollider.hpp"

namespace GFX
{
    SphereCollider::SphereCollider() : Collider()
    {
        this->type = ColliderType::Sphere;
        this->center = Vector3(0, 0, 0);
        this->radius = 0.5f;
    }

    void SphereCollider::SetCenter(const Vector3 &center)
    {
        this->center = center;
    }

    Vector3 SphereCollider::GetCenter() const
    {
        return center;
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