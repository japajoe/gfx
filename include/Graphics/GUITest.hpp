#ifndef GFX_GUITEST_HPP
#define GFX_GUITEST_HPP

#include <cstdint>
#include "Rectangle.hpp"
#include "Graphics2D.hpp"
#include <string>

namespace GFX
{
	class Font;

	class GUITest
	{
	public:
		static void BeginFrame();
		static void EndFrame();
		static bool Button(const Rectangle &rect, const std::string &text);
	private:
		static Font *font;
		static float fontSize;
		static void DrawRectangle(const Rectangle &rect, const Color &color, float rounding);
		static void DrawText(const Vector2 &position, const Rectangle &clippingRect, const Color &color, const std::string &text, float fontSize);
		static Vector2 CalculateTextSize(const std::string &text, float fontSize);
        static Vector2 CalculateCenteredPosition(const Rectangle &rect, Vector2 size);
        static Vector2 GetPositionFromRectangle(const Rectangle &rect);
        static Vector2 GetSizeFromRectangle(const Rectangle &rect);
		static Vector2 GetMousePosition();
		static bool IsMouseDown();
		static bool IsMouseHeld();
		static bool IsMouseReleased();
	};
}

#endif