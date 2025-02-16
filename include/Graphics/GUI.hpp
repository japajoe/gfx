#ifndef GFX_GUI_HPP
#define GFX_GUI_HPP

#include "Font.hpp"
#include "Rectangle.hpp"
#include "Color.hpp"
#include "GUITextBuffer.hpp"
#include "../System/Numerics/Vector2.hpp"
#include "../Core/Input.hpp"
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>

namespace GFX
{
    enum GUIColor
    {
        GUIColor_ButtonNormal,
        GUIColor_ButtonHovered,
        GUIColor_ButtonActive,
        GUIColor_FrameNormal,
        GUIColor_FrameHovered,
        GUIColor_FrameActive,
        GUIColor_DragNormal,
        GUIColor_DragHovered,
        GUIColor_DragActive,
        GUIColor_Text,
        GUIColor_Count
    };

	template<typename T>
	class ItemBuffer
	{
	private:
		std::vector<T> items;
		size_t itemCount;
	public:
		size_t count() const
		{
			return itemCount;
		}

		ItemBuffer()
		{
			items.resize(16);
			itemCount = 0;
		}

		void reset()
		{
			itemCount = 0;
		}

		void add(const T &item)
		{
			if(itemCount >= items.size())
			{
				size_t newCapacity = items.size() * 2;
				items.resize(newCapacity);
			}

			items[itemCount++] = item;
		}

		bool get(size_t index, T &item)
		{
			if(index < items.size())
			{
				item = items[index];
				return true;
			}
			return false;
		}
	};

    struct GUIStyle
    {
        Color colors[GUIColor_Count];
        float buttonRounding;
        float frameRounding;
        float dragRounding;
        float fontSize;

        GUIStyle()
        {
            colors[GUIColor_ButtonNormal] = Color(0.20f, 0.20f, 0.20f, 1.00f);
            colors[GUIColor_ButtonHovered] = Color(0.24f, 0.24f, 0.24f, 1.00f);
            colors[GUIColor_ButtonActive] = Color(0.28f, 0.28f, 0.28f, 1.00f);
            colors[GUIColor_FrameNormal] = Color(0.20f, 0.20f, 0.20f, 1.00f);
            colors[GUIColor_FrameHovered] = Color(0.24f, 0.24f, 0.24f, 1.00f);
            colors[GUIColor_FrameActive] = Color(0.28f, 0.28f, 0.28f, 1.00f);
            colors[GUIColor_DragNormal] = Color(0.48f, 0.48f, 0.48f, 1.00f);
            colors[GUIColor_DragHovered] = Color::SkyBlue();
            colors[GUIColor_DragActive] = Color::SkyBlue();
            colors[GUIColor_Text] = Color::White();

            buttonRounding = 3.0f;
            frameRounding = 3.0f;
            fontSize = 12.0f;
            dragRounding = 3.0f;
        }

        Color GetColor(GUIColor color) const
        {
            return colors[color];
        }
    };

	enum WidgetType
	{
		WidgetType_Box,
		WidgetType_Button,
		WidgetType_CheckBox,
		WidgetType_ComboBox,
		WidgetType_DragFloat,
		WidgetType_InputText,
		WidgetType_Text
	};

	class GUI
	{
	private:
        static int64_t id;
        static int64_t activeId;
        static int64_t hoveredId;
        static int64_t lastHoveredId;
        static int64_t focusedId;
        static bool mousePressed;
        static bool mouseDown;
        static bool mouseUp;
        static Font *font;
		static GUIStyle style;
		static ItemBuffer<char> characterBuffer;
		static ItemBuffer<KeyCode> keyBuffer;
		static std::unordered_map<int64_t, GUITextBuffer> textBuffers;
		static void Initialize();
		static void SetHoveredState(int64_t currentId, bool hovered, bool isMouseDown);
		static bool SetActiveState(int64_t currentId, bool hovered, bool isMouseDown);
		static bool IsActive(int64_t id);
		static bool IsHovered(int64_t id);
		static bool IsFocused(int64_t id);
		static Vector2 GetPositionFromRectangle(const Rectangle &rect);
		static Vector2 GetSizeFromRectangle(const Rectangle &rect);
		static Vector2 CalculateTextSize(const std::string &text, float fontSize);
		static Vector2 CalculateCenteredPosition(const Rectangle &rect, const Vector2 &size);
		static Vector2 GetMousePosition();
		static Vector2 GetMouseDelta();
		static GUITextBuffer *GetTextBuffer(int64_t id);
		static bool IsMouseHovered(const Rectangle &rect);
		static void OnCharPress(uint32_t codepoint);
		static void OnKeyDown(KeyCode keycode);
		static void OnKeyRepeat(KeyCode keycode);
		static bool InputTextInternal(const Rectangle &rect, std::string &text, bool multiLine, bool readOnly);
	public:
		static void BeginFrame();
		static void EndFrame();
		static bool IsMousePressed();
		static bool IsMouseDown();
		static bool IsMouseUp();
		static void Box(const Rectangle &rect);
		static void Text(const Rectangle &rect, const std::string &text);
		static bool Button(const Rectangle &rect, const std::string &text);
		static bool CheckBox(const Rectangle &rect, bool &value);
		static bool DragFloat(const Rectangle &rect, float &value, float min, float max);
		static bool ComboBox(const Rectangle &rect, const std::vector<std::string> &items, int &selectedIndex);
		static Font *GetDefaultFont();
		static Vector2 CalculateTextSize(const std::string &text);
		static float CalculateTextWidth(const std::string &text);
		static float CalculateTextHeight(const std::string &text);
	};
}

#endif