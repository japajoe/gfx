#include "Vertex2D.hpp"

namespace GFX
{
	Vertex2D::Vertex2D()
	{
		this->position = Vector2(0, 0);
		this->uv = Vector2(0, 0);
		this->color = Color(1.0f, 1.0f, 1.0f, 1.0f);
	}

	Vertex2D::Vertex2D(const Vector2 &position, const Vector2 &uv)
	{
		this->position = position;
		this->uv = uv;
		this->color = Color(1.0f, 1.0f, 1.0f, 1.0f);
	}

	Vertex2D::Vertex2D(const Vector2 &position, const Vector2 &uv, const Color &color)
	{
		this->position = position;
		this->uv = uv;
		this->color = color;
	}
}