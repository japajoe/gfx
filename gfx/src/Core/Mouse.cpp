#include "Mouse.hpp"
#include "../../libs/glfw/include/GLFW/glfw3.h"
#include "Application.hpp"

namespace GFX 
{
    Mouse::Mouse() 
	{
        repeatDelay = 0.5; // Delay before repeat starts
        repeatInterval = 0.025; // Interval for repeat
        positionX = 0.0f;
        positionY = 0.0f;
        windowPositionX = 0.0f;
        windowPositionY = 0.0f;
        deltaX = 0.0f;
        deltaY = 0.0f;
        scrollY = 0.0f;
        scrollX = 0.0f;
        cursorVisible = true;

        states[ButtonCode::Left] = ButtonState();
        states[ButtonCode::Right] = ButtonState();
        states[ButtonCode::Middle] = ButtonState();
    }

    Mouse::Mouse(const Mouse &other) 
	{
        repeatDelay = other.repeatDelay;
        repeatInterval = other.repeatInterval;
        positionX = other.positionX;
        positionY = other.positionY;
        windowPositionX = other.windowPositionX;
        windowPositionY = other.windowPositionY;
        deltaX = other.deltaX;
        deltaY = other.deltaY;
        scrollY = other.scrollY;
        scrollX = other.scrollX;
        states = other.states;
        onButtonDown = other.onButtonDown;
        onButtonUp = other.onButtonUp;
        onButtonPress = other.onButtonPress;
        onButtonRepeat = other.onButtonRepeat;
        cursorVisible = other.cursorVisible;
    }

    Mouse::Mouse(Mouse &&other) noexcept 
	{
        repeatDelay = other.repeatDelay;
        repeatInterval = other.repeatInterval;
        positionX = other.positionX;
        positionY = other.positionY;
        windowPositionX = other.windowPositionX;
        windowPositionY = other.windowPositionY;
        deltaX = other.deltaX;
        deltaY = other.deltaY;
        scrollY = other.scrollY;
        scrollX = other.scrollX;
        states = std::move(other.states);
        onButtonDown = other.onButtonDown;
        onButtonUp = other.onButtonUp;
        onButtonPress = other.onButtonPress;
        onButtonRepeat = other.onButtonRepeat;
        cursorVisible = other.cursorVisible;
    }

    Mouse &Mouse::operator=(const Mouse &other) 
	{
        if (this != &other) {
            repeatDelay = other.repeatDelay;
            repeatInterval = other.repeatInterval;
            positionX = other.positionX;
            positionY = other.positionY;
            windowPositionX = other.windowPositionX;
            windowPositionY = other.windowPositionY;
            deltaX = other.deltaX;
            deltaY = other.deltaY;
            scrollY = other.scrollY;
            scrollX = other.scrollX;
            states = other.states;
            onButtonDown = other.onButtonDown;
            onButtonUp = other.onButtonUp;
            onButtonPress = other.onButtonPress;
            onButtonRepeat = other.onButtonRepeat;
            cursorVisible = other.cursorVisible;
        }
        return *this;
    }

    Mouse &Mouse::operator=(Mouse &&other) noexcept 
	{
        if (this != &other) {
            repeatDelay = other.repeatDelay;
            repeatInterval = other.repeatInterval;
            positionX = other.positionX;
            positionY = other.positionY;
            windowPositionX = other.windowPositionX;
            windowPositionY = other.windowPositionY;
            deltaX = other.deltaX;
            deltaY = other.deltaY;
            scrollY = other.scrollY;
            scrollX = other.scrollX;
            states = std::move(other.states);
            onButtonDown = other.onButtonDown;
            onButtonUp = other.onButtonUp;
            onButtonPress = other.onButtonPress;
            onButtonRepeat = other.onButtonRepeat;
            cursorVisible = other.cursorVisible;
        }
        return *this;
    }

    void Mouse::NewFrame() 
	{
        for(auto &k : states) 
		{
            ButtonCode button = k.first;
            int state = k.second.state;

            if(state > 0) 
			{
                if(states[button].down == 0) 
				{
                    states[button].down = 1;
                    states[button].pressed = 0;
                    states[button].lastRepeatTime = glfwGetTime();
                    states[button].repeat = false;
                    if(onButtonDown)
                        onButtonDown(button);
                } 
				else 
				{
                    states[button].down = 1;
                    states[button].pressed = 1;

                    double currentTime = glfwGetTime();
                    double elapsed = currentTime - states[button].lastRepeatTime;

                    if (!states[button].repeat) 
					{
                        if (elapsed >= repeatDelay) 
						{
                            if(onButtonRepeat)
                                onButtonRepeat(button);
                            states[button].repeat = true;
                            states[button].lastRepeatTime = currentTime;
                        }
                    } 
					else 
					{
                        if (elapsed >= repeatInterval) 
						{
                            if(onButtonRepeat)
                                onButtonRepeat(button);
                            states[button].lastRepeatTime = currentTime;
                        }
                    }

                    if(onButtonPress)
                        onButtonPress(button);
                }

                states[button].up = 0;
            } 
			else 
			{
                if(states[button].down == 1 || states[button].pressed == 1) 
				{
                    states[button].down = 0;
                    states[button].pressed = 0;
                    states[button].up = 1;
                    if(onButtonUp)
                        onButtonUp(button);
                } 
				else 
				{
                    states[button].down = 0;
                    states[button].pressed = 0;
                    states[button].up = 0;
                }
            }
        }
    }

    void Mouse::EndFrame() 
	{
        deltaX = 0.0f;
        deltaY = 0.0f;
        scrollX = 0.0f;
        scrollY = 0.0f;
    }

    void Mouse::SetPosition(float x, float y) 
	{
        float prevX = positionX;
        float prevY = positionY;

        positionX = x;
        positionY = y;

        deltaX = x - prevX;
        deltaY = y - prevY;
    }

    void Mouse::SetWindowPosition(float x, float y) 
	{
        windowPositionX = x;
        windowPositionY = y;
    }

    void Mouse::SetScrollDirection(float x, float y) 
	{
        scrollX = x;
        scrollY = y;
    }

    void Mouse::SetCursor(bool visible)
    {
        glfwSetInputMode(Application::GetNativeWindow(), GLFW_CURSOR, visible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
        cursorVisible = visible;
    }

    void Mouse::SetState(ButtonCode buttoncode, int32_t state) 
	{
        states[buttoncode].state = state;
    }

    bool Mouse::GetButton(ButtonCode buttoncode) 
	{
        ButtonState state = states[buttoncode];
        return state.down == 1 && state.pressed == 1;
    }

    bool Mouse::GetButtonDown(ButtonCode buttoncode) 
	{
        ButtonState state = states[buttoncode];
        return state.down == 1 && state.pressed == 0;
    }

    bool Mouse::GetButtonUp(ButtonCode buttoncode) 
	{
        ButtonState state = states[buttoncode];
        return state.up > 0;
    }

    bool Mouse::GetAnyButtonDown(ButtonCode &buttoncode)
    {
        for(auto &item : states)
        {
            if(GetButtonDown(item.first))
            {
                buttoncode = item.first;
                return true;
            }
        }

        return false;
    }

    float Mouse::GetX() const 
	{
        return positionX;
    }

    float Mouse::GetY() const 
	{
        return positionY;
    }

    float Mouse::GetAbsoluteX() const 
	{
        return windowPositionX + positionX;
    }

    float Mouse::GetAbsoluteY() const 
	{
        return windowPositionY + positionY;
    }

    float Mouse::GetDeltaX() const 
	{
        return deltaX;
    }

    float Mouse::GetDeltaY() const 
	{
        return deltaY;
    }

    float Mouse::GetScrollX() const 
	{
        return scrollX;
    }

    float Mouse::GetScrollY() const 
	{
        return scrollY;
    }

    bool Mouse::IsCursorVisible() const
    {
        return cursorVisible;
    }
}