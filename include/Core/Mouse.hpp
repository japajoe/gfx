#ifndef GFX_MOUSE_HPP
#define GFX_MOUSE_HPP

#include <cstdint>
#include <unordered_map>
#include <functional>

namespace GFX 
{
    #define GLFW__MOUSE_BUTTON_1         0
    #define GLFW__MOUSE_BUTTON_2         1
    #define GLFW__MOUSE_BUTTON_3         2
    #define GLFW__MOUSE_BUTTON_4         3
    #define GLFW__MOUSE_BUTTON_5         4
    #define GLFW__MOUSE_BUTTON_6         5
    #define GLFW__MOUSE_BUTTON_7         6
    #define GLFW__MOUSE_BUTTON_8         7
    #define GLFW__MOUSE_BUTTON_LAST      GLFW__MOUSE_BUTTON_8
    #define GLFW__MOUSE_BUTTON_LEFT      GLFW__MOUSE_BUTTON_1
    #define GLFW__MOUSE_BUTTON_RIGHT     GLFW__MOUSE_BUTTON_2
    #define GLFW__MOUSE_BUTTON_MIDDLE    GLFW__MOUSE_BUTTON_3

    enum class ButtonCode 
	{
        Unknown = -1,
        Left = GLFW__MOUSE_BUTTON_LEFT,
        Right = GLFW__MOUSE_BUTTON_RIGHT,
        Middle = GLFW__MOUSE_BUTTON_MIDDLE
    };

    struct ButtonState 
	{
        int down;
        int up;
        int pressed;
        int state;
        double lastRepeatTime;
        bool repeat;

        ButtonState() 
		{
            down = 0;
            up = 0;
            pressed = 0;
            state = 0;
            lastRepeatTime = 0.0;
            repeat = false;
        }
    };

    using MouseButtonDownEvent = std::function<void(ButtonCode buttoncode)>;
    using MouseButtonUpEvent = std::function<void(ButtonCode buttoncode)>;
    using MouseButtonPressEvent = std::function<void(ButtonCode buttoncode)>;
    using MouseButtonRepeatEvent = std::function<void(ButtonCode buttoncode)>;

    class Mouse 
	{
    public:
        MouseButtonDownEvent onButtonDown;
        MouseButtonUpEvent onButtonUp;
        MouseButtonPressEvent onButtonPress;
        MouseButtonRepeatEvent onButtonRepeat;
        Mouse();
        Mouse(const Mouse &other);
        Mouse(Mouse &&other) noexcept;
        Mouse& operator=(const Mouse &other);
        Mouse& operator=(Mouse &&other) noexcept;
        void NewFrame();
        void EndFrame();
        void SetPosition(float x, float y);
        void SetWindowPosition(float x, float y);
        void SetScrollDirection(float x, float y);
        void SetCursor(bool visible);
        void SetState(ButtonCode buttoncode, int32_t state);
        bool GetButton(ButtonCode buttoncode);
        bool GetButtonDown(ButtonCode buttoncode);
        bool GetButtonUp(ButtonCode buttoncode);
        bool GetAnyButtonDown(ButtonCode &keycode);
        float GetX() const;
        float GetY() const;
        float GetAbsoluteX() const;
        float GetAbsoluteY() const;
        float GetDeltaX() const;
        float GetDeltaY() const;
        float GetScrollX() const;
        float GetScrollY() const;
        bool IsCursorVisible() const;
    private:
        std::unordered_map<ButtonCode,ButtonState> states;
        float positionX;
        float positionY;
        float windowPositionX;
        float windowPositionY;
        float deltaX;
        float deltaY;
        float scrollY;
        float scrollX;
        double repeatDelay;
        double repeatInterval;
        bool cursorVisible;
    };
}

#endif