#ifndef GFX_GRAPHICS_HPP
#define GFX_GRAPHICS_HPP

#include "../External/imgui/ImGuiManager.hpp"
#include "../External/glad/glad.h"
#include "../System/EventHandler.hpp"
#include "Buffers/UniformBufferObject.hpp"
#include "Rectangle.hpp"
#include <cstdint>
#include <vector>

namespace GFX
{
	using WindowResizeEvent = std::function<void(uint32_t width, uint32_t height)>;

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
		static EventHandler<WindowResizeEvent> windowResize;
		static Rectangle GetViewport();

		template<typename T>
        static UniformBufferObject CreateUniformBuffer(uint32_t bindingIndex, uint32_t numItems)
        {
            UniformBufferObject ubo;
            ubo.Generate();
            ubo.Bind();

            std::vector<T> data;
			data.resize(numItems);
            
            ubo.BufferData(data.size() * sizeof(T), data.data(), GL_DYNAMIC_DRAW);
            ubo.BindBufferBase(bindingIndex);
            ubo.Unbind();

            return ubo;
        }
	};
}

#endif