#include "GUI.hpp"
#include "../Core/Constants.hpp"
#include "../Core/Resources.hpp"
#include "Graphics2D.hpp"
#include <cstring>

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
	ItemBuffer<char> GUI::characterBuffer;
	ItemBuffer<KeyCode> GUI::keyBuffer;
	std::unordered_map<int64_t, GUITextBuffer> GUI::textBuffers;

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

	static int64_t GetId(const Rectangle &rect, WidgetType type)
	{
		constexpr size_t size = sizeof(Rectangle) + sizeof(WidgetType);
		uint8_t data[size];
		const void *pRect = (const void*)&rect;
		memcpy(data, pRect, sizeof(Rectangle));

		size_t offset = sizeof(Rectangle);
		memcpy(&data[offset], &type, sizeof(WidgetType));
		return GetId(data, size);
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

		characterBuffer.reset();
		keyBuffer.reset();
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

	bool GUI::IsFocused(int64_t id)
	{
		return id == focusedId;// && focusedId > 0;
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

	Vector2 GUI::GetMousePosition()
	{
		return Input::GetMousePosition();
	}

	Vector2 GUI::GetMouseDelta()
	{
		return Input::GetMouseDelta();
	}

	GUITextBuffer *GUI::GetTextBuffer(int64_t id)
	{
		if(!textBuffers.contains(id))
			textBuffers[id] = GUITextBuffer();
		return &textBuffers[id];
	}

	bool GUI::IsMouseHovered(const Rectangle &rect)
	{
		return rect.Contains(Input::GetMousePosition());
	}

	void GUI::Box(const Rectangle &rect)
	{
		Initialize();
		int64_t currentId = GetId(rect, WidgetType_Box);

		Vector2 position = GetPositionFromRectangle(rect);
		Vector2 size = GetSizeFromRectangle(rect);
		Color color = style.GetColor(GUIColor_FrameNormal);

		if(style.frameRounding > 0)
			Graphics2D::AddRectangleRounded(position, size, 0, style.frameRounding, color);
		else
			Graphics2D::AddRectangle(position, size, 0, color);
	}

	void GUI::Text(const Rectangle &rect, const std::string &text)
	{
		Initialize();

		int64_t currentId = GetId(rect, WidgetType_Text);
		Color textColor = style.GetColor(GUIColor_Text);
		Vector2 textPosition = GetPositionFromRectangle(rect);
		Graphics2D::AddText(textPosition, font, text, style.fontSize, textColor, true);
	}

	bool GUI::Button(const Rectangle &rect, const std::string &text)
	{
		Initialize();

		int64_t currentId = GetId(rect, WidgetType_Button);
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

	bool GUI::CheckBox(const Rectangle &rect, bool &value)
	{
		Initialize();

		int64_t currentId = GetId(rect, WidgetType_CheckBox);
		bool hovered = IsMouseHovered(rect);
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

		Color checkColor = value ? style.GetColor(GUIColor_DragActive) : buttonColor;

		Vector2 buttonPosition = GetPositionFromRectangle(rect);
		Vector2 buttonSize = GetSizeFromRectangle(rect);
		
		Rectangle checkRect(rect.x + 4, rect.y + 4, rect.width - 8, rect.height - 8);
		Vector2 checkPosition = GetPositionFromRectangle(checkRect);
		Vector2 checkSize = GetSizeFromRectangle(checkRect);

		if(style.frameRounding > 0)
			Graphics2D::AddRectangleRounded(buttonPosition, buttonSize, 0, style.frameRounding, buttonColor);
		else
			Graphics2D::AddRectangle(buttonPosition, buttonSize, 0, buttonColor);

		Graphics2D::AddRectangle(checkPosition, checkSize, 0, checkColor);

		if(IsHovered(currentId) && IsActive(currentId) && mouseUp)
		{
			value = !value;
			return true;
		}
		return false;
	}

	bool GUI::DragFloat(const Rectangle &rect, float &value, float min, float max)
	{
		Initialize();

		int64_t currentId = GetId(rect, WidgetType_DragFloat);
		bool hovered = IsMouseHovered(rect);
		bool mouseDown = IsMousePressed();
		bool mouseUp = IsMouseUp();

		SetHoveredState(currentId, hovered, mouseDown);
		SetActiveState(currentId, hovered, mouseDown);

		Color backgroundColor = style.GetColor(GUIColor_FrameNormal);
		Color dragColor = style.GetColor(GUIColor_DragNormal);

		if(IsActive(currentId))
		{
			if(IsHovered(currentId) && IsActive(currentId))
			{
				backgroundColor = style.GetColor(GUIColor_FrameActive);
				dragColor = style.GetColor(GUIColor_DragActive);
			}
			else if(IsActive(currentId))
			{
				backgroundColor = style.GetColor(GUIColor_FrameActive);
				dragColor = style.GetColor(GUIColor_DragActive);
			}
			else if(IsHovered(currentId))
			{
				backgroundColor = style.GetColor(GUIColor_FrameHovered);
				dragColor = style.GetColor(GUIColor_DragHovered);
			}
			else
			{
				backgroundColor = style.GetColor(GUIColor_FrameNormal);
				dragColor = style.GetColor(GUIColor_DragNormal);
			}
		}
		else
		{
			if(IsHovered(currentId))
			{
				backgroundColor = style.GetColor(GUIColor_FrameHovered);
				dragColor = style.GetColor(GUIColor_DragHovered);
			}
		}

		Vector2 buttonPosition = GetPositionFromRectangle(rect);
		Vector2 buttonSize = GetSizeFromRectangle(rect);

		auto getGrabRectangle = [] (const Vector2 &position, const Vector2 &size, float value, float valueMin, float valueMax) -> Rectangle {
			Rectangle grabRect(position.x, position.y, size.x, size.y);
			float radius = size.y * 0.5f;
			float width = grabRect.width - (radius * 2.0f);
			float t = (value - valueMin) / (valueMax - valueMin);
			float x = grabRect.x + (t * width);
			float y = (grabRect.y + grabRect.height * 0.5f) - radius;
			return Rectangle(x, y, 2 * radius, 2 * radius);
		};
		
		Rectangle dragRect = getGrabRectangle(buttonPosition, buttonSize, value, min, max);
		Vector2 dragPosition = GetPositionFromRectangle(dragRect);
		Vector2 dragSize = GetSizeFromRectangle(dragRect);

		if(style.frameRounding > 0)
			Graphics2D::AddRectangleRounded(buttonPosition, buttonSize, 0, style.frameRounding, backgroundColor);
		else
			Graphics2D::AddRectangle(buttonPosition, buttonSize, 0, backgroundColor);
		
		if(style.dragRounding > 0)
			Graphics2D::AddRectangleRounded(dragPosition, dragSize, 0, style.dragRounding, dragColor);
		else
			Graphics2D::AddRectangle(dragPosition, dragSize, 0, dragColor);

		if(IsActive(currentId) && focusedId < 0 && mouseDown)
		{
			if(IsMouseHovered(dragRect))
			{
				if(glm::length2(GetMouseDelta()) > 0.0f)
				{
					Vector2 mousePosition = GetMousePosition();
					float normalizedMouseX = (mousePosition.x - buttonPosition.x) / buttonSize.x;
					float sliderValue = min + normalizedMouseX * (max - min);
					sliderValue = glm::clamp(sliderValue, min, max);

					if(value == sliderValue)
						return false;
					
					value = sliderValue;
					
					return true;
				}
			}
			else
			{
				Vector2 mousePosition = GetMousePosition();
				float normalizedMouseX = (mousePosition.x - buttonPosition.x) / buttonSize.x;
				float sliderValue = min + normalizedMouseX * (max - min);
				sliderValue = glm::clamp(sliderValue, min, max);

				if(value == sliderValue)
					return false;
				
				value = sliderValue;
				
				return true;
			}


		}
		return false;
	}

	bool GUI::ComboBox(const Rectangle &rect, const std::vector<std::string> &items, int &selectedIndex)
	{
		Initialize();

		int64_t currentId = GetId(rect, WidgetType_ComboBox);
		bool hovered = IsMouseHovered(rect);
		bool mousePressed = IsMousePressed();
		bool mouseDown = IsMouseDown();
		bool mouseUp = IsMouseUp();

		SetHoveredState(currentId, hovered, mousePressed);
		SetActiveState(currentId, hovered, mousePressed);

		Color buttonColor = style.GetColor(GUIColor_ButtonNormal);
		Color arrowColor = style.GetColor(GUIColor_DragNormal);

		if(IsActive(currentId))
		{
			if(IsHovered(currentId) && IsActive(currentId))
			{
				buttonColor = style.GetColor(GUIColor_ButtonActive);
				arrowColor = style.GetColor(GUIColor_DragActive);
			}
			else if(IsActive(currentId))
			{
				buttonColor = style.GetColor(GUIColor_ButtonActive);
				arrowColor = style.GetColor(GUIColor_DragActive);
			}
			else if(IsHovered(currentId))
			{
				buttonColor = style.GetColor(GUIColor_ButtonHovered);
				arrowColor = style.GetColor(GUIColor_DragHovered);
			}
			else
			{
				buttonColor = style.GetColor(GUIColor_ButtonNormal);
				arrowColor = style.GetColor(GUIColor_DragNormal);
			}
		}
		else
		{
			if(IsHovered(currentId))
			{
				buttonColor = style.GetColor(GUIColor_ButtonHovered);
				arrowColor = style.GetColor(GUIColor_DragHovered);
			}
		}

		float arrowSize = 10;
		float paddingX = 4;
		float paddingY = 4;

		Vector2 pos = GetPositionFromRectangle(rect);
		Vector2 size = GetSizeFromRectangle(rect);
		Vector2 positionBoxMain = pos;
		Vector2 sizeBoxMain(size.x - (arrowSize * 2), size.y);
		float fontMaxSize = font->GetPixelScale(style.fontSize) * font->GetMaxHeight();
		if(sizeBoxMain.y < fontMaxSize)
			sizeBoxMain.y = fontMaxSize;

		Vector2 positionBoxRight(positionBoxMain.x + sizeBoxMain.x, positionBoxMain.y);
		Vector2 sizeBoxRight(arrowSize * 2, sizeBoxMain.y);
		float widgetWidth = sizeBoxMain.x + sizeBoxRight.x;

		Vector2 sizeArrow(arrowSize, arrowSize);
		Vector2 positionArrow(positionBoxRight.x + arrowSize, positionBoxMain.y + sizeArrow.y);

		Vector2 textPosition(pos.x + paddingX, pos.y + paddingY);

		if(style.frameRounding > 0)
		{
			float r = style.frameRounding;
			Graphics2D::AddRectangleRoundedEx(positionBoxMain, sizeBoxMain,   0, 0.0f, r,    0.0f, 0.0f, 0.0f, buttonColor);
			Graphics2D::AddRectangleRoundedEx(positionBoxRight, sizeBoxRight, 0, 0.0f, 0.0f, r, 0.0f, 0.0f, Color::DarkGray());
		}
		else
		{
			Graphics2D::AddRectangle(positionBoxMain, sizeBoxMain,   0, buttonColor);
			Graphics2D::AddRectangle(positionBoxRight, sizeBoxRight, 0, Color::DarkGray());
		}
		
		Graphics2D::AddTriangle(positionArrow, sizeArrow, 0, arrowColor);

		Rectangle clippingRect(positionBoxMain.x, positionBoxMain.y, sizeBoxMain.x, sizeBoxMain.y);

		if(selectedIndex >= 0)
			Graphics2D::AddText(textPosition, font, items[selectedIndex], style.fontSize, Color::White(), false, clippingRect);
		else
			Graphics2D::AddText(textPosition, font, "Select option...", style.fontSize, Color::White(), false, clippingRect);

		if(IsHovered(currentId) && IsActive(currentId) && mouseUp)
		{
			if(currentId == focusedId)
				focusedId = -1;
			else
				focusedId = currentId;
		}

		bool areItemsHovered = false;

		if(currentId == focusedId)
		{
			Vector2 positionOptions(pos.x, pos.y + size.y);
			Vector2 sizeOptions(0, 4);
			Vector2 textOffset(4, 4);

			for(int i = 0; i < items.size(); i++) 
			{
				float width, height;
				font->CalculateBounds(items[i].c_str(), items[i].size(), style.fontSize, width, height);
				width += (textOffset.x * 2); //adds padding for left and right side of text
				height = font->GetMaxHeight() * font->GetPixelScale(style.fontSize);
				if(width > sizeOptions.x)
					sizeOptions.x = width;
				sizeOptions.y += height;
			}

			if(sizeOptions.x < size.x)
				sizeOptions.x = size.x;

			if(style.frameRounding > 0)
			{
				float r = style.frameRounding;
				float tl = 0;
				float tr = widgetWidth < sizeOptions.x ? r : 0.0f;
				float bl = r;
				float br = r;
				Graphics2D::AddRectangleRoundedEx(positionOptions, sizeOptions, 0, 0.0f, tl, tr, bl, br, style.GetColor(GUIColor_FrameNormal));
			}
			else
			{
				Graphics2D::AddRectangle(positionOptions, sizeOptions, 0, style.GetColor(GUIColor_FrameNormal));
			}

			Vector2 mousePosition = GetMousePosition();
			clippingRect = Rectangle(positionOptions.x, positionOptions.y, sizeOptions.x, sizeOptions.y);

			float h = font->GetMaxHeight() * font->GetPixelScale(style.fontSize);
			Vector2 rowPos(clippingRect.x, clippingRect.y + selectedIndex * h);
			Vector2 rowSize(clippingRect.width, h);

			if(selectedIndex >= 0)
				Graphics2D::AddRectangle(rowPos, rowSize, 0, style.GetColor(GUIColor_FrameActive));

			areItemsHovered = IsMouseHovered(clippingRect);

			if(areItemsHovered)
			{
				float my = mousePosition.y;
				float relativeY = my - clippingRect.y;
				int rowIndex = (int)(relativeY / h);

				if(rowIndex < 0)
					rowIndex = 0;

				if(rowIndex >= items.size())
					rowIndex = items.size() -1;

				if(rowIndex < items.size())
				{
					rowPos = Vector2(clippingRect.x, clippingRect.y + rowIndex * h);
					rowSize = Vector2(clippingRect.width, h);

					if(style.frameRounding > 0 && rowIndex == 0 && rowSize.x > widgetWidth)
					{
						float r = style.frameRounding;
						float tl = 0;
						float tr = r;
						float bl = 0;
						float br = 0;
						Graphics2D::AddRectangleRoundedEx(rowPos, rowSize, 0, 0, tl, tr, bl, br, Color::SkyBlue());
					}
					else
					{
						Graphics2D::AddRectangle(rowPos, rowSize, 0, Color::SkyBlue());
					}


					if(mouseUp) 
					{
						selectedIndex = rowIndex;
						focusedId = -1;
						return true;
					}
				}
			}

			std::string text;

			for(int i = 0; i < items.size(); i++)
			{
				text += items[i] + "\n";
			}

			Graphics2D::AddText(Vector2(positionOptions.x + textOffset.x, positionOptions.y + textOffset.y), font, text, style.fontSize, style.GetColor(GUIColor_Text), false, clippingRect);
		}

		if(IsFocused(currentId) && !IsHovered(currentId) && !areItemsHovered)
		{
			if(mouseUp)
				focusedId = -1;
		}

		return false;
	}

	bool GUI::InputTextInternal(const Rectangle &rect, std::string &text, bool multiLine, bool readOnly)
	{
		Initialize();

		int64_t currentId = GetId(rect, WidgetType_InputText);
		bool hovered = IsMouseHovered(rect);
		bool mouseDown = IsMousePressed();
		bool mouseUp = IsMouseUp();
		
		GUITextBuffer *textBuffer = GetTextBuffer(currentId);
		textBuffer->text = text;
		textBuffer->multiLine = multiLine;
		textBuffer->readOnly = readOnly;

		SetHoveredState(currentId, hovered, mouseDown);
		SetActiveState(currentId, hovered, mouseDown);

		Color backgroundColor = style.GetColor(GUIColor_FrameNormal);

		if(IsActive(currentId))
		{
			if(IsHovered(currentId) && IsActive(currentId))
			{
				backgroundColor = style.GetColor(GUIColor_FrameActive);
			}
			else if(IsActive(currentId))
			{
				backgroundColor = style.GetColor(GUIColor_FrameActive);
			}
			else if(IsHovered(currentId))
			{
				backgroundColor = style.GetColor(GUIColor_FrameHovered);
			}
			else
			{
				backgroundColor = style.GetColor(GUIColor_FrameNormal);
			}
		}
		else
		{
			if(IsHovered(currentId))
			{
				backgroundColor = style.GetColor(GUIColor_FrameHovered);
			}
		}

		Vector2 framePosition = GetPositionFromRectangle(rect);
		Vector2 frameSize = GetSizeFromRectangle(rect);

		if(style.frameRounding > 0)
			Graphics2D::AddRectangleRounded(framePosition, frameSize, 0, style.frameRounding, backgroundColor);
		else
			Graphics2D::AddRectangle(framePosition, frameSize, 0, backgroundColor);

		Color textColor = style.GetColor(GUIColor_Text);

		Vector2 textOffset(4, 4);
		Vector2 textPosition = textBuffer->CalculateTextPosition(rect, font, style.fontSize, textOffset);

		Graphics2D::AddText(textPosition, font, textBuffer->text, style.fontSize, textColor, false, rect);
		
		if(IsHovered(currentId) && IsActive(currentId) && mouseUp)
		{
			focusedId = currentId;
		}
		else
		{
			if(currentId == focusedId && mouseUp)
			{
				focusedId = -1;
			}
		}

		if(currentId == focusedId)
		{
			Vector2 cursorPosition = textBuffer->CalculateCursorPosition(rect, font, style.fontSize, textOffset);
			Vector2 cursorSize(2.0f, font->GetLineHeight() * font->GetPixelScale(style.fontSize));

			if(Time::GetTime() >= (textBuffer->lastKeyStroke + textBuffer->blinkInterval))
			{
				float alpha = ((float)glm::cos(textBuffer->blinkTimer * 5.0f) + 1.0f) * 0.5f;
				int on = alpha >= 0.5f ? 1 : 0;
				Graphics2D::AddRectangle(cursorPosition, cursorSize, 0, Color(textColor.r, textColor.g, textColor.b, on * 0.9f), rect);
				textBuffer->blinkTimer += Time::GetDeltaTime();
			} 
			else
			{
				Graphics2D::AddRectangle(cursorPosition, cursorSize, 0, Color(textColor.r, textColor.g, textColor.b, 0.9f), rect);
			}

			// if(keyCommandBuffer.KeyCount > 0 || keyCommandBuffer.CharCount > 0)
			// {
			// 	for(int i = 0; i < keyCommandBuffer.KeyCount; i++)
			// 	{
			// 		if(keyCommandBuffer.GetKey(i, out KeyCode key))
			// 		{
			// 			textBuffer.HandleKeyDown(key);

			// 			if(key == KeyCode.Enter && !textBuffer.multiLine)
			// 			{
			// 				text = textBuffer.text;
			// 				focusedId = -1;
			// 				return true;
			// 			}
			// 		}
			// 	}

			// 	for(int i = 0; i < keyCommandBuffer.CharCount; i++)
			// 	{
			// 		if(keyCommandBuffer.GetCharacter(i, out char character))
			// 		{
			// 			textBuffer.HandleCharPress(character);
			// 		}
			// 	}

			// 	if(text != textBuffer.text)
			// 	{
			// 		text = textBuffer.text;
			// 		return true;
			// 	}
			// }
		}

		return false;
	}

	void GUI::OnCharPress(uint32_t codepoint)
	{
		characterBuffer.add((char)codepoint);
	}

	void GUI::OnKeyDown(KeyCode keycode)
	{
		keyBuffer.add(keycode);
	}

	void GUI::OnKeyRepeat(KeyCode keycode)
	{
		OnKeyDown(keycode);
	}

	Font *GUI::GetDefaultFont()
	{
		return font;
	}

	Vector2 GUI::CalculateTextSize(const std::string &text)
	{
		Initialize();
		Vector2 size;
		font->CalculateBounds(text.c_str(), text.size(), style.fontSize, size.x, size.y);
		return size;
	}

	float GUI::CalculateTextWidth(const std::string &text)
	{
		Initialize();
		Vector2 size;
		font->CalculateBounds(text.c_str(), text.size(), style.fontSize, size.x, size.y);
		return size.x;
	}

	float GUI::CalculateTextHeight(const std::string &text)
	{
		Initialize();
		Vector2 size;
		font->CalculateBounds(text.c_str(), text.size(), style.fontSize, size.x, size.y);
		return size.y;
	}
}