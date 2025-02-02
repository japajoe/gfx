#include "Graphics.hpp"
#include "../External/glad/glad.h"
#include "../External/glfw/glfw3.h"
#include "../Core/Application.hpp"
#include "../External/imgui/imgui.h"
#include "Graphics2D.hpp"

namespace GFX
{
	ImGuiManager Graphics::imgui;
	Rectangle Graphics::viewport;

	void Graphics::Initialize()
	{
		viewport = Rectangle(0, 0, 512, 512);
		imgui.Initialize(Application::GetNativeWindow());
		Graphics2D::Initialize();
	}

	void Graphics::Deinitialize()
	{
		imgui.Deinitialize();
		Graphics2D::Deinitialize();
	}

	void Graphics::NewFrame()
	{
		glClearColor(1, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Graphics2D::NewFrame();

		imgui.BeginFrame();
		
		ImGui::Begin("Test");
		ImGui::End();

		imgui.EndFrame();
	}

	void Graphics::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
		viewport.x = x;
		viewport.y = x;
		viewport.width = width;
		viewport.height = height;
	}

	Rectangle Graphics::GetViewport()
	{
		return viewport;
	}
}