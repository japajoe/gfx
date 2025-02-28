#include "TerrainCollider.hpp"

namespace GFX
{
    TerrainCollider::TerrainCollider() : Collider()
    {
        this->type = ColliderType::Terrain;
    }
}