#include "SphereCollider.hpp"
#include "../../Core/Debug.hpp"
#include "../../Core/Transform.hpp"

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

    void SphereCollider::RenderShape()
    {
        Vector3 pos = center + GetTransform()->GetPosition();
        Debug::DrawSphere(pos, radius, GetTransform()->GetLocalRotation(), Color::Red());
    }
}