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
#include "Shaders/LineShader.hpp"
#include "Shaders/SkyboxShader.hpp"
#include "Shaders/PostProcessingShader.hpp"
#include "Shaders/PostProcessing/HorizontalBlurShader.hpp"
#include "Shaders/PostProcessing/VerticalBlurShader.hpp"
#include "Shaders/PostProcessing/GrayscaleShader.hpp"
#include "Shaders/ParticleShader.hpp"
#include "Shaders/ProceduralSkyboxShader.hpp"
#include "Shaders/ProceduralSkybox2Shader.hpp"
#include "Shaders/TerrainShader.hpp"
#include "Shaders/WaterShader.hpp"
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
#include "../Embedded/JetBrainsMonoRegular.hpp"
#include "../Embedded/DejaVuSansMono.hpp"
#include "../Embedded/ForestGrass.hpp"
#include "Graphics2D.hpp"
#include "Renderers/Renderer.hpp"
#include "Renderers/LineRenderer.hpp"
#include "Renderers/PostProcessingRenderer.hpp"
#include "Materials/DepthMaterial.hpp"

namespace GFX
{
	Rectangle Graphics::viewport;
	Vector2 Graphics::resolution;
	ImGuiManager Graphics::imgui;
	Shadow Graphics::shadow;
	std::unique_ptr<DepthMaterial> Graphics::depthMaterial = nullptr;
	std::vector<Renderer*> Graphics::renderers;
	std::priority_queue<Renderer*, std::vector<Renderer*>, CompareRenderer> Graphics::renderQueue;
	std::vector<FrameBufferObject> Graphics::framebuffers;
	std::vector<Shader*> Graphics::postProcessingShaders;
	PostProcessingRenderer Graphics::postProcessingRenderer;

	void Graphics::Initialize(uint32_t width, uint32_t height, uint32_t displayWidth, uint32_t displayHeight)
	{
		SetViewport(0, 0, width, height);
		resolution = Vector2(displayWidth, displayHeight);

		auto camObject = GameObject::Create();
		camObject->AddComponent<Camera>();

		auto lightObject = GameObject::Create();
		auto light = lightObject->AddComponent<Light>();
		lightObject->GetTransform()->SetPosition(Vector3(100, 100, 100));
		lightObject->GetTransform()->SetRotation(Quaternionf::Euler(glm::radians(45.0f), 0, 0));
		light->SetType(LightType::Directional);
		light->SetStrength(0.1f);

		CreateFonts();
		CreateTextures();
		CreateUniformBuffers();
		CreateShaders();
		CreateMeshes();

		imgui.Initialize(Application::GetNativeWindow());
		Graphics2D::Initialize();
		LineRenderer::Initialize();

		// framebuffers.push_back(FrameBufferObject(width, height));
		// framebuffers.push_back(FrameBufferObject(width, height));
		// framebuffers.push_back(FrameBufferObject(width, height));

		framebuffers.push_back(FrameBufferObject(width, height, true, true));
		framebuffers.push_back(FrameBufferObject(width, height, false, true));
		framebuffers.push_back(FrameBufferObject(width, height, false, true));

		for(size_t i = 0; i < framebuffers.size(); i++)
			framebuffers[i].Generate();

		postProcessingRenderer.Generate();
	}

	void Graphics::Deinitialize()
	{
		imgui.Deinitialize();
		Graphics2D::Deinitialize();
		LineRenderer::Deinitialize();
	}

	void Graphics::NewFrame()
	{
		UpdateUniformBuffers();
		RenderShadowPass();
		Render3DPass();
		RenderPostProcessingPass();
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

        if(renderers.size() > 0 && camera != nullptr)
        {
			auto queue = renderQueue;

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
		framebuffers[0].Bind();
		Clear();

		if(renderers.size() > 0 && Camera::GetMain() != nullptr)
		{
			auto queue = renderQueue;

			while (!queue.empty()) 
			{
				Renderer* currentRenderer = queue.top();
				currentRenderer->OnRender();
				queue.pop();
			}
		}

		LineRenderer::NewFrame();

		framebuffers[0].Unbind();

		//framebuffers[0].Blit(0, viewport.width, viewport.height);

		auto &fbo = framebuffers[1];
		framebuffers[0].Blit(fbo.GetId(), fbo.GetWidth(), fbo.GetHeight());
	}

	void Graphics::RenderPostProcessingPass()
	{
		static Shader *shader = nullptr;

		if(shader == nullptr)
			shader = Resources::FindShader(Constants::GetString(ConstantString::ShaderPostProcessing));

		uint32_t fbo = framebuffers[1].GetId();
		uint32_t texture = framebuffers[1].GetTextureId();

		if(postProcessingShaders.size() > 0)
		{
			uint32_t fbo1 = framebuffers[2].GetId();
			uint32_t texture1 = framebuffers[1].GetTextureId();
			
			uint32_t fbo2 = framebuffers[1].GetId();
			uint32_t texture2 = framebuffers[2].GetTextureId();

			for(size_t i = 0; i < postProcessingShaders.size(); i++)
			{
				postProcessingRenderer.Render(fbo, postProcessingShaders[i]->GetId(), texture);
				
				if(i % 2 == 1)
				{
					fbo = fbo2;
					texture = texture2;
				}
				else
				{
					fbo = fbo1;
					texture = texture1;
				}
			}
		}

		postProcessingRenderer.Render(0, shader->GetId(), texture);
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

		for(size_t i = 0; i < framebuffers.size(); i++)
			framebuffers[i].Resize(width, height);

		windowResize(width, height);
	}

	Rectangle Graphics::GetViewport()
	{
		return viewport;
	}

	void Graphics::CreateUniformBuffers()
	{
		auto sCamera = Constants::GetString(ConstantString::UniformBufferCamera);
		auto sLights = Constants::GetString(ConstantString::UniformBufferLights);
		auto sShadow = Constants::GetString(ConstantString::UniformBufferShadow);
		auto sWorld = Constants::GetString(ConstantString::UniformBufferWorld);

		//Create uniform buffers
		auto uboCamera = UniformBufferObject::Create<UniformCameraInfo>(UniformBindingIndex_Camera, 1);
		auto uboLights = UniformBufferObject::Create<UniformLightInfo>(UniformBindingIndex_Lights, Light::MAX_LIGHTS);
		auto uboShadow = UniformBufferObject::Create<UniformShadowInfo>(UniformBindingIndex_Shadow, 1);
		auto uboWorld = UniformBufferObject::Create<UniformWorldInfo>(UniformBindingIndex_World, 1);

		uboCamera.ObjectLabel(sCamera);
		uboLights.ObjectLabel(sLights);
		uboShadow.ObjectLabel(sShadow);
		uboWorld.ObjectLabel(sWorld);

		Resources::AddUniformBuffer(sCamera, uboCamera);
		Resources::AddUniformBuffer(sLights, uboLights);
		Resources::AddUniformBuffer(sShadow, uboShadow);
		Resources::AddUniformBuffer(sWorld, uboWorld);
	}

	void Graphics::CreateShaders()
	{
		//Create shaders
		auto diffuseShader = Resources::AddShader(Constants::GetString(ConstantString::ShaderDiffuse), DiffuseShader::Create());
		auto depthShader = Resources::AddShader(Constants::GetString(ConstantString::ShaderDepth), DepthShader::Create());
		auto lineShader = Resources::AddShader(Constants::GetString(ConstantString::ShaderLine), LineShader::Create());
		auto skyboxShader = Resources::AddShader(Constants::GetString(ConstantString::ShaderSkybox), SkyboxShader::Create());
		auto proceduralSkyboxShader = Resources::AddShader(Constants::GetString(ConstantString::ShaderProceduralSkybox), ProceduralSkyboxShader::Create());
		auto proceduralSkyboxShader2 = Resources::AddShader(Constants::GetString(ConstantString::ShaderProceduralSkybox2), ProceduralSkybox2Shader::Create());
		auto terrainShader = Resources::AddShader(Constants::GetString(ConstantString::ShaderTerrain), TerrainShader::Create());
		auto waterShader = Resources::AddShader(Constants::GetString(ConstantString::ShaderWater), WaterShader::Create());
		auto particleShader = Resources::AddShader(Constants::GetString(ConstantString::ShaderParticle), ParticleShader::Create());
		auto postProcessingShader = Resources::AddShader(Constants::GetString(ConstantString::ShaderPostProcessing), PostProcessingShader::Create());
		auto horizontalBlurShader = Resources::AddShader(Constants::GetString(ConstantString::ShaderHorizontalBlur), HorizontalBlurShader::Create());
		auto verticalBlurShader = Resources::AddShader(Constants::GetString(ConstantString::ShaderVerticalBlur), VerticalBlurShader::Create());
		auto grayscaleShader = Resources::AddShader(Constants::GetString(ConstantString::ShaderGrayscale), GrayscaleShader::Create());

		BindShaderToUniformBuffers(diffuseShader);
		BindShaderToUniformBuffers(depthShader);
		BindShaderToUniformBuffers(lineShader);
		BindShaderToUniformBuffers(skyboxShader);
		BindShaderToUniformBuffers(proceduralSkyboxShader);
		BindShaderToUniformBuffers(proceduralSkyboxShader2);
		BindShaderToUniformBuffers(terrainShader);
		BindShaderToUniformBuffers(waterShader);
		BindShaderToUniformBuffers(particleShader);
		BindShaderToUniformBuffers(postProcessingShader);
		BindShaderToUniformBuffers(horizontalBlurShader);
		BindShaderToUniformBuffers(verticalBlurShader);
		BindShaderToUniformBuffers(grayscaleShader);

		depthMaterial = std::make_unique<DepthMaterial>();

		shadow.Generate();
	}

	void Graphics::CreateTextures()
	{
		//Create default textures
		auto defaultTexture = Resources::AddTexture2D(Constants::GetString(ConstantString::TextureDefault), Texture2D(2, 2, Color::White()));
		auto defaultCubemap = Resources::AddTextureCubeMap(Constants::GetString(ConstantString::TextureDefaultCubeMap), TextureCubeMap(2, 2, Color::White()));
		
		Image imageGrass(ForestGrass::GetData(), ForestGrass::GetSize());
		if(imageGrass.IsLoaded())
		{
			auto defaultGrass = Resources::AddTexture2D(Constants::GetString(ConstantString::TextureDefaultGrass), Texture2D(&imageGrass));
			defaultGrass->ObjectLabel("TextureDefaultGrass");
		}

		defaultTexture->ObjectLabel("TextureDefault");
		defaultCubemap->ObjectLabel("TextureDefaultCubeMap");
	}

	void Graphics::CreateFonts()
	{
		//Create default font
		Font font;
		if(font.LoadFromMemory(DejaVuSansMono::GetData(), DejaVuSansMono::GetSize(), 32, FontRenderMethod::SDF))
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
		Resources::AddMesh(Constants::GetString(ConstantString::MeshCylinder), MeshGenerator::CreateCylinder(Vector3f::One()));
		Resources::AddMesh(Constants::GetString(ConstantString::MeshPlane), MeshGenerator::CreatePlane(Vector3f::One()));
		Resources::AddMesh(Constants::GetString(ConstantString::MeshQuad), MeshGenerator::CreateQuad(Vector3f::One()));
		Resources::AddMesh(Constants::GetString(ConstantString::MeshSphere), MeshGenerator::CreateSphere(Vector3f::One()));
		Resources::AddMesh(Constants::GetString(ConstantString::MeshSkybox), MeshGenerator::CreateSkybox(Vector3f::One()));
	}

	void Graphics::BindShaderToUniformBuffers(Shader *shader)
	{
		if(shader == nullptr)
		{
			Debug::WriteLine("Can't bind shader to uniform buffers because shader is null");
			return;
		}

		auto sCamera = Constants::GetString(ConstantString::UniformBufferCamera);
		auto sLights = Constants::GetString(ConstantString::UniformBufferLights);
		auto sShadow = Constants::GetString(ConstantString::UniformBufferShadow);
		auto sWorld = Constants::GetString(ConstantString::UniformBufferWorld);

		auto uboCamera = Resources::FindUniformBuffer(sCamera);
		auto uboLights = Resources::FindUniformBuffer(sLights);
		auto uboShadow = Resources::FindUniformBuffer(sShadow);
		auto uboWorld = Resources::FindUniformBuffer(sWorld);

		uboCamera->BindBlockToShader(shader->GetId(), UniformBindingIndex_Camera, sCamera);
		uboLights->BindBlockToShader(shader->GetId(), UniformBindingIndex_Lights, sLights);
		uboShadow->BindBlockToShader(shader->GetId(), UniformBindingIndex_Shadow, sShadow);
		uboWorld->BindBlockToShader(shader->GetId(), UniformBindingIndex_World, sWorld);
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

	void Graphics::AddPostProcessingShader(Shader *shader)
	{
        if(!shader)
            return;

        postProcessingShaders.push_back(shader);
	}

	void Graphics::RemovePostProcessingShader(Shader *shader)
	{
        if(!shader)
            return;

        size_t index = 0;
        bool found = false;

        for(size_t i = postProcessingShaders.size() -1; i >= 0; i--)
        {
            if(postProcessingShaders[i] == shader)
            {
                index = i;
                found = true;
                break;
            }
        }

        if(found)
        {
            postProcessingShaders.erase(postProcessingShaders.begin() + index);
        }
	}

    Renderer *Graphics::GetRendererByIndex(size_t index)
    {
        if(index >= renderers.size())
            return nullptr;
        return renderers[index];
    }

	FrameBufferObject *Graphics::GetFrameBufferByIndex(size_t index)
	{
        if(index >= framebuffers.size())
            return nullptr;
        return &framebuffers[index];
	}
}