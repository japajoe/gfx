#include "GUILayout.hpp"

namespace GFX
{
	Vector2 GUILayout::mainCursor = Vector2(5, 5);
	Vector2 GUILayout::padding = Vector2(10, 10);
	Rectangle GUILayout::lastRect;
	bool GUILayout::sameLine = false;

	void GUILayout::SetCursorInternal(const Rectangle &rect)
	{
		auto cursor = GetCursor();
		cursor.y += rect.height + (padding.y / 2);
		SetCursor(cursor);
		lastRect = rect;
	}

	Vector2 GUILayout::GetCursorInternal()
	{
		Vector2 cursor = GetCursor();
		if(!sameLine)
		{
			cursor.x = padding.x / 2.0f;
			SetCursor(cursor);
		}
		sameLine = false;
		return cursor;
	}

	Vector2 GUILayout::GetCursor()
	{
		return mainCursor;
	}

	float GUILayout::GetCursorX()
	{
		return mainCursor.x;
	}

	float GUILayout::GetCursorY()
	{
		return mainCursor.y;
	}

	void GUILayout::SetCursor(const Vector2 &value)
	{
		mainCursor = value;
	}

	void GUILayout::SetCursorX(float value)
	{
		mainCursor.x = value;
	}

	void GUILayout::SetCursorY(float value)
	{
		mainCursor.y = value;
	}

	Rectangle GUILayout::GetLastRectangle()
	{
		return lastRect;
	}

	void GUILayout::SameLine()
	{
		auto cursor  = GetCursor();
		cursor.x += lastRect.width + (padding.x / 2);
		cursor.y -= lastRect.height + (padding.y / 2);
		SetCursor(cursor);
		sameLine = true;
	}

	void GUILayout::BeginFrame()
	{
		GUI::BeginFrame();
		SetCursor(Vector2(5, 5));
	}

	void GUILayout::EndFrame()
	{
		GUI::EndFrame();
	}

	void GUILayout::Box(const Vector2 &size)
	{
		Vector2 cursor = GetCursorInternal();
		Rectangle rect = Rectangle(cursor.x, cursor.y, size.x, size.y);

		SetCursorInternal(rect);

		GUI::Box(rect);
	}

	void GUILayout::Text(const std::string &text)
	{
		Vector2 cursor = GetCursorInternal();
		Vector2 size = GUI::CalculateTextSize(text);

		auto font = GUI::GetDefaultFont();
		float maxHeight = font->GetPixelScale(16) * font->GetMaxHeight();

		if(size.y < maxHeight)
			size.y = maxHeight;

		Rectangle rect(cursor.x, cursor.y, size.x, size.y);

		GUI::Text(rect, text);

		SetCursorInternal(rect);
	}

	bool GUILayout::Button(const std::string &text)
	{
		Vector2 cursor = GetCursorInternal();
		Vector2 minSize = Vector2(80, 20) + padding;
		Vector2 size = GUI::CalculateTextSize(text) + padding;
		Rectangle rect;
		
		if(size.x > minSize.x || size.y > minSize.y)
			rect = Rectangle(cursor.x, cursor.y, size.x, size.y);
		else
			rect = Rectangle(cursor.x, cursor.y, minSize.x, minSize.y);

		SetCursorInternal(rect);

		return GUI::Button(rect, text);
	}

	bool GUILayout::CheckBox(bool &value)
	{
		Vector2 cursor = GetCursorInternal();
		Vector2 minSize = Vector2(8, 8) + padding;
		Rectangle rect(cursor.x, cursor.y, minSize.x, minSize.y);

		SetCursorInternal(rect);

		return GUI::CheckBox(rect, value);
	}

	bool GUILayout::DragFloat(float &value, float min, float max)
	{
		Vector2 cursor = GetCursorInternal();
		Vector2 minSize(100, 20);
		Rectangle rect(cursor.x, cursor.y, minSize.x, minSize.y);

		SetCursorInternal(rect);

		return GUI::DragFloat(rect, value, min, max);
	}

	bool GUILayout::ComboBox(const std::vector<std::string> &items, int &selectedIndex)
	{
		Vector2 cursor = GetCursorInternal();
		Vector2 minSize(100, 20);
		Rectangle rect(cursor.x, cursor.y, minSize.x, minSize.y);

		SetCursorInternal(rect);

		return GUI::ComboBox(rect, items, selectedIndex);
	}
}