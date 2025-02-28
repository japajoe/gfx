#include "Vertex.hpp"

namespace GFX
{
    Vertex::Vertex()
    {
        this->position = Vector3(0, 0, 0);
        this->normal = Vector3(0, 0, 0);
        this->uv = Vector2(0, 0);
    }

    Vertex::Vertex(const Vector3 &position, const Vector3 &normal, const Vector2 &uv)
    {
        this->position = position;
        this->normal = normal;
        this->uv = uv;
    }
}