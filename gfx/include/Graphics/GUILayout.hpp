#ifndef GFX_GUILAYOUT_HPP
#define GFX_GUILAYOUT_HPP

#include "GUI.hpp"

namespace GFX
{
	class GUILayout
	{
	private:
		static Vector2 mainCursor;
        static Vector2 padding;
        static Rectangle lastRect;
        static bool sameLine;
        static void SetCursorInternal(const Rectangle &rect);
        static Vector2 GetCursorInternal();
	public:
		static Vector2 GetCursor();
		static float GetCursorX();
		static float GetCursorY();
		static void SetCursor(const Vector2 &value);
		static void SetCursorX(float value);
		static void SetCursorY(float value);
		static Rectangle GetLastRectangle();
		static void SameLine();
		static void BeginFrame();
		static void EndFrame();
		static void Box(const Vector2 &size);
		static void Text(const std::string &text);
		static bool Button(const std::string &text);
		static bool CheckBox(bool &value);
		static bool DragFloat(float &value, float min, float max);
		static bool ComboBox(const std::vector<std::string> &items, int &selectedIndex);
	};
}

#endif