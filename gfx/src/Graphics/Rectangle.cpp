#include "Rectangle.hpp"

namespace GFX
{
	Rectangle::Rectangle()
	{
		this->x = 0;
		this->y = 0;
		this->width = 0;
		this->height = 0;
	}

	Rectangle::Rectangle(float x, float y, float width, float height)
	{
		this->x = x;
		this->y = y;
		this->width = width;
		this->height = height;
	}

	Vector2 Rectangle::GetCenter() const
	{
		float centerX = x + (width / 2);
		float centerY = y + (height / 2);
		return Vector2(centerX, centerY);
	}

	bool Rectangle::IsZero() const
	{
		return x == 0.0f && y == 0.0f && width == 0.0f && height == 0.0f;
	}

	bool Rectangle::Contains(const Vector2 &point) const
	{
		return point.x >= x && point.x <= x + width && point.y >= y && point.y <= y + height;
	}

	Vector2 Rectangle::GetPosition() const
	{
		return Vector2(x, y);
	}

	Vector2 Rectangle::GetSizeFromRectangle() const
	{
		return Vector2(width, height);
	}
	
	Rectangle Rectangle::GetRectAtRowAndColumn(float leftIndent, float topIndent, float width, float height, int row, int column, int offsetX, int offsetY)
	{
		float x = leftIndent + (column * (width + offsetX));
		float y = topIndent + (row * (height + offsetY));
		return Rectangle(x, y, width, height);
	}
}