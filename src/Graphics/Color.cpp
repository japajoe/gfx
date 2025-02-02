#include "Color.hpp"

namespace GFX
{
	Color::Color()
	{
		this->r = 1.0;
		this->g = 1.0;
		this->b = 1.0;
		this->a = 1.0;
	}

	Color::Color(float r, float g, float b, float a)
	{
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}

	Color::Color(float r, float g, float b)
	{
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = 1.0f;
	}

	Color::Color(int r, int g, int b, int a)
	{
		this->r = Normalize(r);
		this->g = Normalize(g);
		this->b = Normalize(b);
		this->a = Normalize(a);
	}

	float Color::Normalize(int value)
	{
		if(value <= 0)
			return 0;
		return (float)value / 255.0f;
	}
}
