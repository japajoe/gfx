#include "Graphics.hpp"
#include "Texture2D.hpp"
#include "Texture3D.hpp"
#include "Shader.hpp"
#include "Font.hpp"
#include "Buffers/UniformBufferObject.hpp"
#include "Shaders/DiffuseShader.hpp"
#include "Shaders/DepthShader.hpp"
#include "../External/glad/glad.h"
#include "../Core/Application.hpp"
#include "../Core/Constants.hpp"
#include "../Core/Debug.hpp"
#include "../Core/GameBehaviour.hpp"
#include "../Core/GameObject.hpp"
#include "../Core/Camera.hpp"
#include "../Core/Light.hpp"
#include "../Core/Resources.hpp"
#include "../Embedded/RobotoMonoRegular.hpp"
#include "Graphics2D.hpp"

namespace GFX
{
	Rectangle Graphics::viewport;
	ImGuiManager Graphics::imgui;
	Shadow Graphics::shadow;

	void Graphics::Initialize(uint32_t width, uint32_t height)
	{
		SetViewport(0, 0, width, height);

		imgui.Initialize(Application::GetNativeWindow());
		Graphics2D::Initialize();

		auto camObject = GameObject::Create();
		camObject->AddComponent<Camera>();

		auto lightObject = GameObject::Create();
		lightObject->AddComponent<Light>();

		//Create default font
		Font font;
		if(font.LoadFromMemory(RobotoMonoRegular::GetData(), RobotoMonoRegular::GetSize(), 32, FontRenderMethod::SDF))
		{
			if(font.GenerateTexture())
			{				
				Resources::AddFont("Default", font);
			}
		}
		//Create default textures
		Resources::AddTexture2D("Default", Texture2D(2, 2, Color::White()));
		Resources::AddTexture3D("Depth", Texture3D(2048, 2048, 5));

		//Create shaders
		auto diffuseShader = Resources::AddShader("Diffuse", DiffuseShader::Create());
		auto depthShader = Resources::AddShader("Depth", DepthShader::Create());

		//Create uniform buffers
		auto uboCamera = UniformBufferObject::Create<UniformCameraInfo>(UniformBindingIndex_Camera, 1);
		auto uboLights = UniformBufferObject::Create<UniformLightInfo>(UniformBindingIndex_Lights, Light::MAX_LIGHTS);
		auto uboShadow = UniformBufferObject::Create<UniformShadowInfo>(UniformBindingIndex_Shadows, 1);

		Resources::AddUniformBuffer("Camera", uboCamera);
		Resources::AddUniformBuffer("Lights", uboLights);
		Resources::AddUniformBuffer("Shadow", uboShadow);

		BindShaderToUniformBuffers(diffuseShader);
		BindShaderToUniformBuffers(depthShader);

		shadow.Generate();
	}

	void Graphics::Deinitialize()
	{
		imgui.Deinitialize();
		Graphics2D::Deinitialize();
	}

	void Graphics::NewFrame()
	{
		Clear();
		UpdateUniformBuffers();
		RenderShadowPass();
		Render3DPass();
		Render2DPass();
	}

	void Graphics::UpdateUniformBuffers()
	{
		Camera::UpdateUniformBuffer();
		Light::UpdateUniformBuffer();
		shadow.UpdateUniformBuffer();
	}

	void Graphics::RenderShadowPass()
	{

	}

	void Graphics::Render3DPass()
	{

	}

	void Graphics::Render2DPass()
	{
		Graphics2D::NewFrame();
		imgui.BeginFrame();
		GameBehaviour::OnBehaviourGUI();
		imgui.EndFrame();
	}

	void Graphics::Clear()
	{
		auto camera = Camera::GetMain();
		if(camera != nullptr)
		{
			auto color = camera->GetClearColor();
			glClearColor(color.r, color.g, color.b, color.a);
		}	
		else
		{
			glClearColor(0.05, 0.05, 0.05, 1.0);
		}
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void Graphics::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
		viewport.x = x;
		viewport.y = x;
		viewport.width = width;
		viewport.height = height;

		windowResize(width, height);
	}

	Rectangle Graphics::GetViewport()
	{
		return viewport;
	}

	void Graphics::BindShaderToUniformBuffers(Shader *shader)
	{
		if(shader == nullptr)
		{
			Debug::WriteLine("Can't bind shader to uniform buffers because shader is null");
			return;
		}

		auto uboCamera = Resources::FindUniformBuffer("Camera");
		auto uboLights = Resources::FindUniformBuffer("Lights");

		uboCamera->BindBlockToShader(shader->GetId(), UniformBindingIndex_Camera, "Camera");
		uboLights->BindBlockToShader(shader->GetId(), UniformBindingIndex_Lights, "Lights");
	}
}