#ifndef GFX_GRAPHICS_HPP
#define GFX_GRAPHICS_HPP

#include "../External/imgui/ImGuiManager.hpp"
#include "../External/glad/glad.h"
#include "../System/EventHandler.hpp"
#include "Renderers/Renderer.hpp"
#include "Renderers/PostProcessingRenderer.hpp"
#include "Buffers/FrameBufferObject.hpp"
#include "Shader.hpp"
#include "Shadow.hpp"
#include "Rectangle.hpp"
#include <cstdint>
#include <vector>
#include <queue>
#include <memory>

namespace GFX
{
	using WindowResizeEvent = std::function<void(uint32_t width, uint32_t height)>;

	class DepthMaterial;

	class Graphics
	{
	friend class Application;
	private:
		static Rectangle viewport;
		static Vector2 resolution;
		static ImGuiManager imgui;
		static Shadow shadow;
		static std::unique_ptr<DepthMaterial> depthMaterial;
		static std::vector<Renderer*> renderers;
		static std::priority_queue<Renderer*, std::vector<Renderer*>, CompareRenderer> renderQueue;
		static std::vector<FrameBufferObject> framebuffers;
		static std::vector<Shader*> postProcessingShaders;
		static PostProcessingRenderer postProcessingRenderer;
		static void Initialize(uint32_t width, uint32_t height, uint32_t displayWidth, uint32_t displayHeight);
		static void Deinitialize();
		static void NewFrame();
		static void UpdateUniformBuffers();
		static void RenderShadowPass();
		static void Render2DPass();
		static void Render3DPass();
		static void RenderPostProcessingPass();
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
		static void AddPostProcessingShader(Shader *shader);
		static void RemovePostProcessingShader(Shader *shader);
		static Renderer *GetRendererByIndex(size_t index);
		static FrameBufferObject *GetFrameBufferByIndex(size_t index);
	};
}

#endif