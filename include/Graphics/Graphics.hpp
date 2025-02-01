#ifndef GFX_GRAPHICS_HPP
#define GFX_GRAPHICS_HPP

#include "../External/imgui/ImGuiManager.hpp"
#include <cstdint>

namespace GFX
{
	class Graphics
	{
	friend class Application;
	private:
		static ImGuiManager imgui;
		static void Initialize();
		static void Deinitialize();
		static void NewFrame();
		static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
	};
}

#endif