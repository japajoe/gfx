#include "Input.hpp"
#include "../External/glfw/glfw3.h"
#include "Application.hpp"

namespace GFX
{
	Keyboard Input::keyboard;
	Mouse Input::mouse;

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

	void Input::AddInputCharacter(uint codepoint)
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
}