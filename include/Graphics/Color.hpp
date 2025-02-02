#ifndef GFX_COLOR_HPP
#define GFX_COLOR_HPP

namespace GFX
{
	struct Color
	{
		float r;
		float g;
		float b;
		float a;
		Color();
		Color(float r, float g, float b, float a);
		Color(float r, float g, float b);
		Color(int r, int g, int b, int a);
		float Normalize(int value);
	};
}

#endif