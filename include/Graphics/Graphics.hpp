#ifndef GFX_GRAPHICS_HPP
#define GFX_GRAPHICS_HPP

#include "../External/imgui/ImGuiManager.hpp"
#include "Rectangle.hpp"
#include <cstdint>

namespace GFX
{
	class Graphics
	{
	friend class Application;
	private:
		static ImGuiManager imgui;
		static Rectangle viewport;
		static void Initialize();
		static void Deinitialize();
		static void NewFrame();
		static void Render2DPass();
		static void Render3DPass();
		static void Clear();
		static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
	public:
		static Rectangle GetViewport();
	};
}

#endif