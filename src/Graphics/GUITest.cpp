#include "GUITest.hpp"
#include "Font.hpp"
#include "../Core/Resources.hpp"
#include "../Core/Constants.hpp"
#include "../Core/Input.hpp"

namespace GFX
{
	struct InputState 
	{
		bool mouseDown;
		bool mousePressed;
		int mouseX, mouseY;
	};

	Font *GUITest::font = nullptr;
	float GUITest::fontSize = 14;

	static Color NormalColor = Color::DarkGray();
	static Color HoverColor(0.8f, 0.8f, 0.8f);
	static Color ActiveColor(0.5f, 0.5f, 0.5f);
	static Color FocusedColor(0.3f, 0.5f, 0.8f);
	static Vector2 TextPadding(5, 5);

	static int64_t GetId(const void *data, size_t size) 
	{
		uint8_t *d = (uint8_t*)data;
		int64_t hash = 0;
		int c;

		for(size_t i = 0; i < size; i++) 
		{
			c = d[i];
			hash = c + (hash << 6) + (hash << 16) - hash;
		}

		return hash;
	}

	void GUITest::BeginFrame()
	{
		if(font == nullptr)
			font = Resources::FindFont(Constants::GetString(ConstantString::FontDefault));

		if(font == nullptr)
			return;
	}

	void GUITest::EndFrame()
	{

	}

	bool GUITest::Button(const Rectangle &rect, const std::string &text)
	{
		int64_t id = GetId(text.c_str(), text.size());
		bool hovered = rect.Contains(GetMousePosition());
		bool clicked = false;

		if (hovered && IsMouseReleased())
		{
			clicked = true;
		}

		Vector2 position = GetPositionFromRectangle(rect);
		Vector2 textSize = CalculateTextSize(text, fontSize);
		Vector2 textPosition = position + CalculateCenteredPosition(rect, textSize);

		Color bgColor = hovered ? (IsMouseDown ? ActiveColor : HoverColor) : NormalColor;
		DrawRectangle(rect, bgColor, 4.0);
		DrawText(textPosition, rect, Color::White(), text, fontSize);

		return clicked;
	}

	void GUITest::DrawRectangle(const Rectangle &rect, const Color &color, float rounding)
	{
		Vector2 position = GetPositionFromRectangle(rect);
		Vector2 size = GetSizeFromRectangle(rect);
		if(rounding > 0)
			Graphics2D::AddRectangleRounded(position, size, 0, rounding, color);
		else
			Graphics2D::AddRectangle(position, size, 0, color);
	}

	void GUITest::DrawText(const Vector2 &position, const Rectangle &clippingRect, const Color &color, const std::string &text, float fontSize)
	{
		Graphics2D::AddText(position, font, text, fontSize, color, false, clippingRect);
	}

	Vector2 GUITest::CalculateTextSize(const std::string &text, float fontSize)
	{
		Vector2 bounds;
		font->CalculateBounds(text.c_str(), text.size(), fontSize, bounds.x, bounds.y);
		return bounds;
	}

	Vector2 GUITest::CalculateCenteredPosition(const Rectangle &rect, Vector2 size)
	{
		return Vector2((rect.width - size.x) * 0.5f, (rect.height - size.y) * 0.5f);
	}

	Vector2 GUITest::GetPositionFromRectangle(const Rectangle &rect)
	{
		return Vector2(rect.x, rect.y);
	}

	Vector2 GUITest::GetSizeFromRectangle(const Rectangle &rect)
	{
		return Vector2(rect.width, rect.height);
	}

	Vector2 GUITest::GetMousePosition()
	{
		return Input::GetMousePosition();
	}

	bool GUITest::IsMouseDown()
	{
		return Input::GetButtonDown(ButtonCode::Left);
	}

	bool GUITest::IsMouseHeld()
	{
		return Input::GetButton(ButtonCode::Left);
	}

	bool GUITest::IsMouseReleased()
	{
		return Input::GetButtonUp(ButtonCode::Left);
	}
}