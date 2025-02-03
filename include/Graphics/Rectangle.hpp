#ifndef GFX_RECTANGLE_HPP
#define GFX_RECTANGLE_HPP

#include "../External/glm/glm.hpp"

namespace GFX
{
	struct Rectangle
	{
		float x;
		float y;
		float width;
		float height;
		Rectangle();
		Rectangle(float x, float y, float width, float height);
		Vector2 GetCenter() const;
		bool IsZero() const;
		bool Contains(const Vector2 &point)const;
        Vector2 GetPosition() const;
        Vector2 GetSizeFromRectangle() const;
		static Rectangle GetRectAtRowAndColumn(float leftIndent, float topIndent, float width, float height, int row, int column, int offsetX = 0, int offsetY = 0);
	};
}

#endif