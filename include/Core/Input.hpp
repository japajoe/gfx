#ifndef GFX_INPUT_HPP
#define GFX_INPUT_HPP

#include "Keyboard.hpp"
#include "Mouse.hpp"
#include "../External/glm/glm.hpp"
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <string>

namespace GFX
{
    class AxisKeys
    {
    public:
        KeyCode positive;
        KeyCode negative;
        AxisKeys();
        AxisKeys(KeyCode positive, KeyCode negative);
    };

    class AxisInfo
    {
    public:
        std::string name;
        std::vector<AxisKeys> keys;
        AxisInfo();
        AxisInfo(const std::string &name);
        void AddKeys(KeyCode positive, KeyCode negative);
    };

    class Input
    {
	friend class Application;
	private:
        static Keyboard keyboard;
        static Mouse mouse;
        static std::unordered_map<std::string, AxisInfo> keyToAxisMap;
        static void NewFrame();
        static void EndFrame();
        static void SetMousePosition(double x, double y);
        static void SetWindowPosition(double x, double y);
        static void SetKeyState(KeyCode keycode, int state);
        static void AddInputCharacter(uint32_t codepoint);
        static void SetButtonState(ButtonCode buttoncode, int state);
        static void SetScrollDirection(double x, double y);
	public:
        static void Initialize();
        static void RegisterAxis(const AxisInfo &axisInfo);
        static float GetAxis(const std::string &axis);
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
        static Keyboard *GetKeyboard();
        static Mouse *GetMouse();
    };
}

#endif