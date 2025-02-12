#include "CylinderCollider.hpp"

namespace GFX
{
    CylinderCollider::CylinderCollider() : Collider()
    {
        this->type = ColliderType::Cylinder;
        this->height = 1.0f;
        this->radius = 0.5f;
    }

    void CylinderCollider::SetHeight(float height)
    {
        this->height = height;
    }

    float CylinderCollider::GetHeight() const
    {
        return height;
    }

    void CylinderCollider::SetRadius(float radius)
    {
        this->radius = radius;
    }

    float CylinderCollider::GetRadius() const
    {
        return radius;
    }
}