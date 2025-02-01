#include "Keyboard.hpp"
#include "Mouse.hpp"
#include "../External/glm/glm.hpp"
#include <cstdint>

namespace GFX
{
    class Input
    {
	friend class Application;
	private:
        static Keyboard keyboard;
        static Mouse mouse;
        static void NewFrame();
        static void EndFrame();
        static void SetMousePosition(double x, double y);
        static void SetWindowPosition(double x, double y);
        static void SetKeyState(KeyCode keycode, int state);
        static void AddInputCharacter(uint32_t codepoint);
        static void SetButtonState(ButtonCode buttoncode, int state);
        static void SetScrollDirection(double x, double y);
	public:
        static bool GetKey(KeyCode keycode);
        static bool GetKeyDown(KeyCode keycode);
        static bool GetKeyUp(KeyCode keycode);
        static bool GetAnyKeyDown(KeyCode &keycode);
        static bool GetButton(ButtonCode buttoncode);
        static bool GetButtonDown(ButtonCode buttoncode);
        static bool GetButtonUp(ButtonCode buttoncode);
        static bool GetAnyButtonDown(ButtonCode &buttoncode);
        static Vector2 GetScrollDirection();
        static Vector2 GetMousePosition();
        static Vector2 GetMouseDelta();
        static void SetMouseCursor(bool visible);
        static bool IsCursorVisible();
    };
}