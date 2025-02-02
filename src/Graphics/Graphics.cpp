#include "Graphics.hpp"
#include "../External/glad/glad.h"
#include "../External/glfw/glfw3.h"
#include "../Core/Application.hpp"
#include "../Core/GameBehaviour.hpp"
#include "../Core/GameObject.hpp"
#include "../Core/Camera.hpp"
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

		auto camObject = GameObject::Create();
		camObject->AddComponent<Camera>();

		//auto uboCamera = CreateUniformBuffer<UniformCameraInfo>(Camera::UBO_BINDING_INDEX, 1);
	}

	void Graphics::Deinitialize()
	{
		imgui.Deinitialize();
		Graphics2D::Deinitialize();
	}

	void Graphics::NewFrame()
	{
		Clear();
		Render3DPass();
		Render2DPass();
	}

	void Graphics::Render2DPass()
	{
		Graphics2D::NewFrame();
		imgui.BeginFrame();
		GameBehaviour::OnBehaviourGUI();
		imgui.EndFrame();
	}

	void Graphics::Render3DPass()
	{

	}

	void Graphics::Clear()
	{
		glClearColor(0.05, 0.05, 0.05, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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