#include "GUI.hpp"
#include "../Core/Constants.hpp"
#include "../Core/Resources.hpp"
#include "Graphics2D.hpp"

namespace GFX
{
	int64_t GUI::id = 0;
	int64_t GUI::activeId = -1;
	int64_t GUI::hoveredId = -1;
	int64_t GUI::lastHoveredId = -1;
	int64_t GUI::focusedId = -1;
	bool GUI::mousePressed = false;
	bool GUI::mouseDown = false;
	bool GUI::mouseUp = false;
	Font *GUI::font;
	GUIStyle GUI::style;
	ItemBuffer<char> GUI::keyCommandBuffer;

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

	void GUI::Initialize()
	{
		if(font != nullptr)
			return;

		font = Resources::FindFont(Constants::GetString(ConstantString::FontDefault));

		Input::GetKeyboard()->onKeyDown += OnKeyDown;
		Input::GetKeyboard()->onKeyRepeat += OnKeyRepeat;
		Input::GetKeyboard()->onCharPress += OnCharPress;
	}

	void GUI::BeginFrame()
	{
		id = 0;
		hoveredId = 0;
		mousePressed = Input::GetButton(ButtonCode::Left);
		mouseDown = Input::GetButtonDown(ButtonCode::Left);
		mouseUp = Input::GetButtonUp(ButtonCode::Left);
	}

	void GUI::EndFrame()
	{
		if(!IsMousePressed())
		{
			activeId = 0;
		}

		keyCommandBuffer.reset();
	}

	bool GUI::IsMousePressed()
	{
		return mousePressed;
	}

	bool GUI::IsMouseDown()
	{
		return mouseDown;
	}

	bool GUI::IsMouseUp()
	{
		return mouseUp;
	}

	void GUI::SetHoveredState(int64_t currentId, bool hovered, bool isMouseDown)
	{
		if(focusedId > 0 && focusedId != currentId)
			return;

		if(hovered)
		{
			hoveredId = currentId;

			if(lastHoveredId != hoveredId)
			{
				lastHoveredId = hoveredId;
			}
		}
		else
		{
			if(lastHoveredId == currentId)
			{
				lastHoveredId = -1;
			}
		}
	}

	bool GUI::SetActiveState(int64_t currentId, bool hovered, bool isMouseDown)
	{
		if(hovered)
		{
			if(activeId == 0 && isMouseDown)
			{
				activeId = currentId;

				return true;
			}
		}

		return false;
	}

	bool GUI::IsActive(int64_t id)
	{
		return id == activeId;
	}

	bool GUI::IsHovered(int64_t id)
	{
		return id == hoveredId;
	}

	Vector2 GUI::GetPositionFromRectangle(const Rectangle &rect)
	{
		return Vector2(rect.x, rect.y);
	}

	Vector2 GUI::GetSizeFromRectangle(const Rectangle &rect)
	{
		return Vector2(rect.width, rect.height);
	}

	Vector2 GUI::CalculateTextSize(const std::string &text, float fontSize)
	{
		Vector2 bounds;
		font->CalculateBounds(text.c_str(), text.size(), fontSize, bounds.x, bounds.y);
		return bounds;
	}

	Vector2 GUI::CalculateCenteredPosition(const Rectangle &rect, const Vector2 &size)
	{
		return Vector2((rect.width - size.x) * 0.5f, (rect.height - size.y) * 0.5f);
	}

	bool GUI::IsMouseHovered(const Rectangle &rect)
	{
		return rect.Contains(Input::GetMousePosition());
	}

	bool GUI::Button(const Rectangle &rect, const std::string &text)
	{
		Initialize();

		int64_t currentId = GetId(&rect, sizeof(Rectangle));
		bool hovered = rect.Contains(Input::GetMousePosition());
		bool mouseDown = IsMousePressed();
		bool mouseUp = IsMouseUp();

		SetHoveredState(currentId, hovered, mouseDown);
		SetActiveState(currentId, hovered, mouseDown);

		Color buttonColor = style.GetColor(GUIColor_ButtonNormal);

		if(IsActive(currentId))
		{
			if(IsHovered(currentId) && IsActive(currentId))
			{
				buttonColor = style.GetColor(GUIColor_ButtonActive);
			}
			else if(IsActive(currentId))
			{
				buttonColor = style.GetColor(GUIColor_ButtonActive);
			}
			else if(IsHovered(currentId))
			{
				buttonColor = style.GetColor(GUIColor_ButtonHovered);
			}
			else
			{
				buttonColor = style.GetColor(GUIColor_ButtonNormal);
			}
		}
		else
		{
			if(IsHovered(currentId))
			{
				buttonColor = style.GetColor(GUIColor_ButtonHovered);
			}
		}

		Vector2 buttonPosition = GetPositionFromRectangle(rect);
		Vector2 buttonSize = GetSizeFromRectangle(rect);
		Vector2 textSize = CalculateTextSize(text, style.fontSize);
		Vector2 textPosition = buttonPosition + CalculateCenteredPosition(rect, textSize);

		if(style.buttonRounding > 0)
			Graphics2D::AddRectangleRounded(buttonPosition, buttonSize, 0, style.buttonRounding, buttonColor);
		else
			Graphics2D::AddRectangle(buttonPosition, buttonSize, 0, buttonColor);

		Color textColor = style.GetColor(GUIColor_Text);
		
		Graphics2D::AddText(textPosition, font, text, style.fontSize, textColor, false, rect);

		if(hoveredId == currentId && activeId == currentId && mouseUp)
			return true;
		return false;
	}

	void GUI::OnCharPress(uint32_t codepoint)
	{

	}

	void GUI::OnKeyDown(KeyCode keycode)
	{

	}

	void GUI::OnKeyRepeat(KeyCode keycode)
	{

	}
}