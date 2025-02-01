#include "Graphics.hpp"
#include "../External/glad/glad.h"
#include "../External/glfw/glfw3.h"
#include "../Core/Application.hpp"
#include "../External/imgui/imgui.h"

namespace GFX
{
	ImGuiManager Graphics::imgui;

	void Graphics::Initialize()
	{
		imgui.Initialize(Application::GetNativeWindow());
	}

	void Graphics::Deinitialize()
	{
		imgui.Deinitialize();
	}

	void Graphics::NewFrame()
	{
		glClearColor(1, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		imgui.BeginFrame();
		
		ImGui::Begin("Test");
		ImGui::End();

		imgui.EndFrame();
	}

	void Graphics::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
	}
}