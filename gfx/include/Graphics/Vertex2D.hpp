#ifndef GFX_VERTEX2D_HPP
#define GFX_VERTEX2D_HPP

#include "../External/glm/glm.hpp"
#include "Color.hpp"

namespace GFX
{
	struct Vertex2D
	{
		Vector2 position;
		Vector2 uv;
		Color color;
		Vertex2D();
		Vertex2D(const Vector2 &position, const Vector2 &uv);
		Vertex2D(const Vector2 &position, const Vector2 &uv, const Color &color);
	};
}

#endif