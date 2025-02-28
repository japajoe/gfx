#include "MeshCollider.hpp"

namespace GFX
{
    MeshCollider::MeshCollider() : Collider()
    {
        this->type = ColliderType::Mesh;
        this->mesh = nullptr;
    }

	void MeshCollider::SetMesh(Mesh *mesh)
	{
		this->mesh = mesh;
	}

	Mesh *MeshCollider::GetMesh() const
	{
		return mesh;
	}
}