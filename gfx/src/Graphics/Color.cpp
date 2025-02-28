#include "Color.hpp"
#include "../External/glm/glm.hpp"

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

	Color Color::Lerp(const Color &a, const Color &b, float t)
    {
        Color c;
        c.r = glm::lerp(a.r, b.r, t);
        c.g = glm::lerp(a.g, b.g, t);
        c.b = glm::lerp(a.b, b.b, t);
        c.a = glm::lerp(a.a, b.a, t);
        return c;
    }

    Color Color::ToGrayScale(const Color &color)
    {
        float v = (color.r + color.g + color.b);
        if(v > 0.0f)
            v /= 3.0f;
        Color c;
        c.r = v;
        c.g = v;
        c.b = v;
        c.a = color.a;
        return c;
    }

    Color Color::LightGray()
    {
        return { 200, 200, 200, 255 };
    }

    Color Color::Gray()
    {
        return { 130, 130, 130, 255 };
    }

    Color Color::DarkGray()
    {
        return { 80, 80, 80, 255 };
    }

    Color Color::Yellow()
    {
        return { 253, 249, 0, 255 };
    }

    Color Color::Gold()
    {
        return { 255, 203, 0, 255 };
    }

    Color Color::Orange()
    {
        return { 255, 161, 0, 255 };
    }

    Color Color::Pink()
    {
        return { 255, 109, 194, 255 };
    }

    Color Color::Red()
    {
        return { 255, 0, 0, 255 };
    }

    Color Color::Maroon()
    {
        return { 190, 33, 55, 255 };
    }

    Color Color::Green()
    {
        return { 0, 255, 0, 255 };
    }

    Color Color::Lime()
    {
        return { 0, 158, 47, 255 };
    }

    Color Color::LightGreen()
    {
        return { 13, 224, 77, 255 };
    }

    Color Color::DarkGreen()
    {
        return { 0, 117, 44, 255 };
    }

    Color Color::SkyBlue()
    {
        return { 102, 191, 255, 255 };
    }

    Color Color::Blue()
    {
        return { 0, 0, 255, 255 };
    }

    Color Color::DarkBlue()
    {
        return { 0, 82, 172, 255 };
    }

    Color Color::Purple()
    {
        return { 200, 122, 255, 255 };
    }

    Color Color::Violet()
    {
        return { 135, 60, 190, 255 };
    }

    Color Color::DarkPurple()
    {
        return { 112, 31, 126, 255 };
    }

    Color Color::Beige()
    {
        return { 211, 176, 131, 255 };
    }

    Color Color::Brown()
    {
        return { 127, 106, 79, 255 };
    }

    Color Color::DarkBrown()
    {
        return { 76, 63, 47, 255 };
    }

    Color Color::White()
    {
        return { 255, 255, 255, 255 };
    }

    Color Color::Black()
    {
        return { 0, 0, 0, 255 };
    }

    Color Color::Blank()
    {
        return { 0, 0, 0, 0 };
    }

    Color Color::Cyan()
    {
        return { 0, 255, 255, 255 };
    }

    Color Color::Magenta()
    {
        return { 255, 0, 255, 255 };
    }

    Color Color::RayWhite()
    {
        return { 245, 245, 245, 255 };
    }
}