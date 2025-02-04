#ifndef GFX_GRAPHICS_HPP
#define GFX_GRAPHICS_HPP

#include "../External/imgui/ImGuiManager.hpp"
#include "../External/glad/glad.h"
#include "../System/EventHandler.hpp"
#include "Shadow.hpp"
#include "Rectangle.hpp"
#include <cstdint>
#include <vector>
#include <queue>
#include <memory>

namespace GFX
{
	using WindowResizeEvent = std::function<void(uint32_t width, uint32_t height)>;

	class Shader;
	class Renderer;
	class DepthMaterial;

    struct CompareRendererOrder 
    {
        bool operator()(const Renderer *lhs, const Renderer *rhs) const;
    };

	class Graphics
	{
	friend class Application;
	private:
		static Rectangle viewport;
		static ImGuiManager imgui;
		static Shadow shadow;
		static std::unique_ptr<DepthMaterial> depthMaterial;
		static std::vector<Renderer*> renderers;
		static std::priority_queue<Renderer*, std::vector<Renderer*>, CompareRendererOrder> renderQueue;
		static void Initialize(uint32_t width, uint32_t height);
		static void Deinitialize();
		static void NewFrame();
		static void UpdateUniformBuffers();
		static void RenderShadowPass();
		static void Render2DPass();
		static void Render3DPass();
		static void Clear();
		static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
		static void BindShaderToUniformBuffers(Shader *shader);
		static void CreateUniformBuffers();
		static void CreateShaders();
		static void CreateTextures();
		static void CreateFonts();
		static void CreateMeshes();
	public:
		static EventHandler<WindowResizeEvent> windowResize;
		static Rectangle GetViewport();
		static void Add(Renderer *renderer);
		static void Remove(Renderer *renderer);
	};
}

#endif