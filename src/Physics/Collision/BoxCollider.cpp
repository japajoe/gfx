#include "BoxCollider.hpp"
#include "../../Core/Debug.hpp"
#include "../../Core/Transform.hpp"

namespace GFX
{
    BoxCollider::BoxCollider() : Collider()
    {
        this->type = ColliderType::Box;
        this->size = Vector3(1, 1, 1);
    }

    void BoxCollider::SetSize(const Vector3 &size)
    {
        this->size = size;
    }

    Vector3 BoxCollider::GetSize() const
    {
        return size;
    }

    void BoxCollider::RenderShape()
    {
        Debug::DrawCube(center + GetTransform()->GetPosition(), size, GetTransform()->GetLocalRotation(), Color::Red());
    }
}