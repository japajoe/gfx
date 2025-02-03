#include "Graphics.hpp"
#include "Texture2D.hpp"
#include "Texture3D.hpp"
#include "Shader.hpp"
#include "Font.hpp"
#include "Mesh.hpp"
#include "World.hpp"
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
#include "Renderers/Renderer.hpp"
#include "Materials/DepthMaterial.hpp"

namespace GFX
{
	Rectangle Graphics::viewport;
	ImGuiManager Graphics::imgui;
	Shadow Graphics::shadow;
	std::unique_ptr<DepthMaterial> Graphics::depthMaterial = nullptr;
	std::vector<Renderer*> Graphics::renderers;
	std::priority_queue<Renderer*, std::vector<Renderer*>, CompareRendererOrder> Graphics::renderQueue;

    bool CompareRendererOrder::operator()(const Renderer *lhs, const Renderer *rhs) const 
    {
        // Lower rendering order gets priority
        return lhs->GetRenderOrder() > rhs->GetRenderOrder();
    }

	void Graphics::Initialize(uint32_t width, uint32_t height)
	{
		SetViewport(0, 0, width, height);

		imgui.Initialize(Application::GetNativeWindow());
		Graphics2D::Initialize();

		auto camObject = GameObject::Create();
		camObject->AddComponent<Camera>();

		auto lightObject = GameObject::Create();
		auto light = lightObject->AddComponent<Light>();
		lightObject->GetTransform()->SetPosition(Vector3(100, 100, 100));
		lightObject->GetTransform()->LookAt(camObject->GetTransform());
		light->SetType(LightType::Directional);
		light->SetStrength(0.1f);

		CreateFonts();
		CreateTextures();
		CreateShaders();
		CreateMeshes();
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
		World::UpdateUniformBuffer();
		shadow.UpdateUniformBuffer();
	}

	void Graphics::RenderShadowPass()
	{
		if(!Shadow::IsEnabled())
			return;

		auto camera = Camera::GetMain();

        if(renderQueue.size() > 0 && camera != nullptr)
        {
            std::priority_queue<Renderer*, std::vector<Renderer*>, CompareRendererOrder> queue = renderQueue;

			shadow.Bind();

            while (!queue.empty()) 
            {
                Renderer* renderer = queue.top();
				if(renderer->GetCastShadows())
				{
                	renderer->OnRender(depthMaterial.get(), camera);
				}
                queue.pop();
            }

			shadow.Unbind();
        }
	}

	void Graphics::Render3DPass()
	{
        if(renderQueue.size() > 0 && Camera::GetMain() != nullptr)
        {
            std::priority_queue<Renderer*, std::vector<Renderer*>, CompareRendererOrder> queue = renderQueue;

            while (!queue.empty()) 
            {
                Renderer* currentRenderer = queue.top();
                currentRenderer->OnRender();
                queue.pop();
            }
        }
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

		std::string sCamera = Constants::GetString(ConstantString::UniformBufferCamera);
		std::string sLights = Constants::GetString(ConstantString::UniformBufferLights);
		std::string sShadow = Constants::GetString(ConstantString::UniformBufferShadow);
		std::string sWorld = Constants::GetString(ConstantString::UniformBufferWorld);

		auto uboCamera = Resources::FindUniformBuffer(sCamera);
		auto uboLights = Resources::FindUniformBuffer(sLights);
		auto uboShadow = Resources::FindUniformBuffer(sShadow);
		auto uboWorld = Resources::FindUniformBuffer(sWorld);

		glObjectLabel(GL_BUFFER, uboCamera->GetId(), -1, sCamera.c_str());
		glObjectLabel(GL_BUFFER, uboLights->GetId(), -1, sLights.c_str());
		glObjectLabel(GL_BUFFER, uboShadow->GetId(), -1, sShadow.c_str());
		glObjectLabel(GL_BUFFER, uboWorld->GetId(), -1, sWorld.c_str());

		uboCamera->BindBlockToShader(shader->GetId(), UniformBindingIndex_Camera, sCamera);
		uboLights->BindBlockToShader(shader->GetId(), UniformBindingIndex_Lights, sLights);
		uboShadow->BindBlockToShader(shader->GetId(), UniformBindingIndex_Shadow, sShadow);
		uboWorld->BindBlockToShader(shader->GetId(), UniformBindingIndex_World, sWorld);
	}

	void Graphics::CreateShaders()
	{
		//Create shaders
		auto diffuseShader = Resources::AddShader(Constants::GetString(ConstantString::ShaderDiffuse), DiffuseShader::Create());
		auto depthShader = Resources::AddShader(Constants::GetString(ConstantString::ShaderDepth), DepthShader::Create());

		//Create uniform buffers
		auto uboCamera = UniformBufferObject::Create<UniformCameraInfo>(UniformBindingIndex_Camera, 1);
		auto uboLights = UniformBufferObject::Create<UniformLightInfo>(UniformBindingIndex_Lights, Light::MAX_LIGHTS);
		auto uboShadow = UniformBufferObject::Create<UniformShadowInfo>(UniformBindingIndex_Shadow, 1);
		auto uboWorld = UniformBufferObject::Create<UniformWorldInfo>(UniformBindingIndex_World, 1);

		Resources::AddUniformBuffer(Constants::GetString(ConstantString::UniformBufferCamera), uboCamera);
		Resources::AddUniformBuffer(Constants::GetString(ConstantString::UniformBufferLights), uboLights);
		Resources::AddUniformBuffer(Constants::GetString(ConstantString::UniformBufferShadow), uboShadow);
		Resources::AddUniformBuffer(Constants::GetString(ConstantString::UniformBufferWorld), uboWorld);

		BindShaderToUniformBuffers(diffuseShader);
		BindShaderToUniformBuffers(depthShader);

		depthMaterial = std::make_unique<DepthMaterial>();

		shadow.Generate();
	}

	void Graphics::CreateTextures()
	{
		//Create default textures
		auto defaultTexture = Resources::AddTexture2D(Constants::GetString(ConstantString::TextureDefault), Texture2D(2, 2, Color::White()));
		auto depthTexture = Resources::AddTexture3D(Constants::GetString(ConstantString::TextureDepth), Texture3D(2048, 2048, 5));

		defaultTexture->ObjectLabel("TextureDefault");
		depthTexture->ObjectLabel("TextureDepth");
	}

	void Graphics::CreateFonts()
	{
		//Create default font
		Font font;
		if(font.LoadFromMemory(RobotoMonoRegular::GetData(), RobotoMonoRegular::GetSize(), 32, FontRenderMethod::SDF))
		{
			if(font.GenerateTexture())
			{				
				Resources::AddFont(Constants::GetString(ConstantString::FontDefault), font);
			}
		}
	}

	void Graphics::CreateMeshes()
	{
		Resources::AddMesh(Constants::GetString(ConstantString::MeshCapsule), MeshGenerator::CreateCapsule(Vector3f::One()));
		Resources::AddMesh(Constants::GetString(ConstantString::MeshCube), MeshGenerator::CreateCube(Vector3f::One()));
		Resources::AddMesh(Constants::GetString(ConstantString::MeshPlane), MeshGenerator::CreatePlane(Vector3f::One()));
		Resources::AddMesh(Constants::GetString(ConstantString::MeshQuad), MeshGenerator::CreateQuad(Vector3f::One()));
		Resources::AddMesh(Constants::GetString(ConstantString::MeshSphere), MeshGenerator::CreateSphere(Vector3f::One()));
		Resources::AddMesh(Constants::GetString(ConstantString::MeshSkybox), MeshGenerator::CreateSkybox(Vector3f::One()));
	}

	void Graphics::Add(Renderer *renderer)
	{
        if(!renderer)
            return;

        for(size_t i = 0; i < renderers.size(); i++)
        {
            if(renderers[i] == renderer)
            {
                Debug::WriteError("[RENDERER] can't add with ID: %llu because it already exists", renderer->GetInstanceId());
                return;
            }
        }

        Debug::WriteLog("[RENDERER] added with ID: %llu", renderer->GetInstanceId());

        renderers.push_back(renderer);

        renderQueue.push(renderer);
	}
	
	void Graphics::Remove(Renderer *renderer)
	{
        if(!renderer)
            return;

        size_t index = 0;
        bool found = false;

        for(size_t i = 0; i < renderers.size(); i++)
        {
            if(renderers[i] == renderer)
            {
                index = i;
                found = true;
                break;
            }
        }

        if(found)
        {
            Debug::WriteLog("[RENDERER] removed with ID: %llu", renderer->GetInstanceId());
            renderers.erase(renderers.begin() + index);

            //Clear render queue and recreate
            while(!renderQueue.empty())
            {
                renderQueue.pop();
            }

            for(size_t i = 0; i < renderers.size(); i++)
            {
                renderQueue.push(renderers[i]);
            }
        }
	}
}