#include "Input.hpp"
#include "../../libs/glfw/include/GLFW/glfw3.h"
#include "Application.hpp"

namespace GFX
{
    AxisKeys::AxisKeys()
    {

    }

    AxisKeys::AxisKeys(KeyCode positive, KeyCode negative)
    {
        this->positive = positive;
        this->negative = negative;
    }

    AxisInfo::AxisInfo()
    {

    }

    AxisInfo::AxisInfo(const std::string &name)
    {
        this->name = name;
    }

    void AxisInfo::AddKeys(KeyCode positive, KeyCode negative)
    {
        this->keys.push_back(AxisKeys(positive, negative));
    }

	Keyboard Input::keyboard;
	Mouse Input::mouse;
	std::unordered_map<std::string, AxisInfo> Input::keyToAxisMap;

	void Input::Initialize()
	{
        AxisInfo axisHorizontal("Horizontal");
        AxisInfo axisVertical("Vertical");
        AxisInfo axisPanning("Panning");

        axisHorizontal.AddKeys(KeyCode::D, KeyCode::A);
        axisVertical.AddKeys(KeyCode::W, KeyCode::S);
        axisPanning.AddKeys(KeyCode::R, KeyCode::F);
        
        RegisterAxis(axisHorizontal);
        RegisterAxis(axisVertical);
        RegisterAxis(axisPanning);
	}

	void Input::NewFrame()
	{
		//Cursorpos callback is not called when mouse is outside the bounds of the window
		double cursorPosX, cursorPosY;
		glfwGetCursorPos(Application::GetNativeWindow(), &cursorPosX, &cursorPosY);
		SetMousePosition(cursorPosX, cursorPosY);

		keyboard.NewFrame();
		mouse.NewFrame();
	}

	void Input::EndFrame()
	{
		mouse.EndFrame();
	}

	void Input::SetMousePosition(double x, double y)
	{
		mouse.SetPosition(x, y);
	}

	void Input::SetWindowPosition(double x, double y)
	{
		mouse.SetWindowPosition(x, y);
	}

	void Input::SetKeyState(KeyCode keycode, int state)
	{
		keyboard.SetState(keycode, state);
	}

	void Input::AddInputCharacter(uint32_t codepoint)
	{
		keyboard.AddInputCharacter(codepoint);
	}

	void Input::SetButtonState(ButtonCode buttoncode, int state)
	{
		mouse.SetState(buttoncode, state);
	}

	void Input::SetScrollDirection(double x, double y)
	{
		mouse.SetScrollDirection(x, y);
	}

    void Input::RegisterAxis(const AxisInfo &axisInfo)
    {
        if(keyToAxisMap.count(axisInfo.name) > 0)
            return;
        keyToAxisMap[axisInfo.name] = axisInfo;
    }

    float Input::GetAxis(const std::string &axis)
    {
        if (keyToAxisMap.count(axis) > 0)
        {
            for (size_t i = 0; i < keyToAxisMap[axis].keys.size(); i++)
            {
                if (GetKey(keyToAxisMap[axis].keys[i].positive))
                    return 1.0f;
                else if (GetKey(keyToAxisMap[axis].keys[i].negative))
                    return -1.0f;
            }
        }

        return 0.0f;
    }

	bool Input::GetKey(KeyCode keycode)
	{
		return keyboard.GetKey(keycode);
	}

	bool Input::GetKeyDown(KeyCode keycode)
	{
		return keyboard.GetKeyDown(keycode);
	}

	bool Input::GetKeyUp(KeyCode keycode)
	{
		return keyboard.GetKeyUp(keycode);
	}

	bool Input::GetAnyKeyDown(KeyCode &keycode)
	{
		return keyboard.GetAnyKeyDown(keycode);
	}

	bool Input::GetButton(ButtonCode buttoncode)
	{
		return mouse.GetButton(buttoncode);
	}

	bool Input::GetButtonDown(ButtonCode buttoncode)
	{
		return mouse.GetButtonDown(buttoncode);
	}

	bool Input::GetButtonUp(ButtonCode buttoncode)
	{
		return mouse.GetButtonUp(buttoncode);
	}

	bool Input::GetAnyButtonDown(ButtonCode &buttoncode)
	{
		return mouse.GetAnyButtonDown(buttoncode);
	}

	Vector2 Input::GetScrollDirection()
	{
		return Vector2(mouse.GetScrollX(), mouse.GetScrollY());
	}

	Vector2 Input::GetMousePosition()
	{
		return Vector2(mouse.GetX(), mouse.GetY());
	}

	Vector2 Input::GetMouseDelta()
	{
		return Vector2(mouse.GetDeltaX(), mouse.GetDeltaY());
	}

	void Input::SetMouseCursor(bool visible)
	{
		mouse.SetCursor(visible);
	}

	bool Input::IsCursorVisible()
	{
		return mouse.IsCursorVisible();
	}

	Keyboard *Input::GetKeyboard()
	{
		return &keyboard;
	}

	Mouse *Input::GetMouse()
	{
		return &mouse;
	}
}