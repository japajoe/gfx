#ifndef GFX_GUITEXTBUFFER_HPP
#define GFX_GUITEXTBUFFER_HPP

#include "Rectangle.hpp"
#include "Font.hpp"
#include "../System/Numerics/Vector2.hpp"
#include "../External/glm/glm.hpp"
#include "../Core/Keyboard.hpp"
#include "../Core/Time.hpp"
#include <string>

namespace GFX
{
    struct GUITextBuffer
    {
        enum class TextType
        {
            Normal,
            NumericFloat,
            NumericInteger
        };

        struct TextOptions
        {
            TextType textType;
            float value;
            float min;
            float max;
        };

        TextOptions options;
        std::string text;
        int cursorIndex;
        long widgetId;
        float blinkTimer;
        float blinkInterval;
        float lastKeyStroke;
        bool multiLine;
        bool readOnly;
        GUITextBuffer();
        void SetCursorIndex(int index);
        void GetCursorPosition(int &row, int &column);
		void SetCursorPosition(int row, int column);
        void ResetLastKeyStroke(float elapsedTime) ;
        Vector2 CalculateTextPosition(const Rectangle &rect, Font *font, float fontSize, const Vector2 &textOffset);
        Vector2 CalculateCursorPosition(const Rectangle &rect, Font *font, float fontSize, const Vector2 &textOffset);
        int GetIndexOfNextNewLineChar();
        int GetIndexOfPreviousNewLineChar();
		void HandleKeyDown(KeyCode keycode);
    };
}

#endif