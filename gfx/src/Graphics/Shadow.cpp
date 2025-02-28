#include "Shadow.hpp"
#include "Texture3D.hpp"
#include "Buffers/ElementBufferObject.hpp"
#include "Graphics.hpp"
#include "../Core/Resources.hpp"
#include "../Core/Debug.hpp"
#include "../Core/Camera.hpp"
#include "../Core/Light.hpp"
#include "../Core/Constants.hpp"

namespace GFX
{
	bool Shadow::enabled = true;

	Shadow::Shadow()
	{
        depthMap = nullptr;
		camera = nullptr;
		light = nullptr;
	}

	void Shadow::Generate()
	{
        size_t cascadeCount = 4;
        depthMap = Resources::AddTexture3D(Constants::GetString(ConstantString::TextureDepth), Texture3D(2048, 2048, cascadeCount + 1));
        depthMap->ObjectLabel("TextureDepth");

		camera = Camera::GetMain();
		light = Light::GetMain();

		float farPlane = 1000.0f;

        shadowCascadeLevels.resize(cascadeCount);
        
        shadowCascadeLevels[0] = farPlane / 50.0f;
        shadowCascadeLevels[1] = farPlane / 25.0f;
        shadowCascadeLevels[2] = farPlane / 10.0f;
        shadowCascadeLevels[3] = farPlane / 2.0f;

		shadowData.cascadeCount = shadowCascadeLevels.size();
		shadowData.farPlane = farPlane;
		shadowData.shadowBias = 0.005f;
		shadowData.enabled = enabled ? 1 : 0;

		lightFBO = 0;

        glGenFramebuffers(1, &lightFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, lightFBO);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthMap->GetId(), 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

        if (status != GL_FRAMEBUFFER_COMPLETE)
        {
            Debug::WriteError("CascadedShadowMap framebuffer is not complete");
            throw 0;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Shadow::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, lightFBO);
		glViewport(0, 0, depthMap->GetWidth(), depthMap->GetHeight());
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_FRONT);  // peter panning
	}

	void Shadow::Unbind()
	{
		auto viewport = Graphics::GetViewport();
		glCullFace(GL_BACK);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, (int)viewport.width, (int)viewport.height);
	}

    bool Shadow::IsEnabled()
    {
        return enabled;
    }

    void Shadow::SetEnabled(bool enabled)
    {
        Shadow::enabled = enabled;
    }

	std::vector<Vector4> Shadow::GetFrustumCornersWorldSpace(const Matrix4 &projview)
	{
        const auto inv = Matrix4f::Invert(projview);

        std::vector<Vector4> frustumCorners;
        for (unsigned int x = 0; x < 2; ++x)
        {
            for (unsigned int y = 0; y < 2; ++y)
            {
                for (unsigned int z = 0; z < 2; ++z)
                {
                    const Vector4 pt = inv * Vector4(2.0f * x - 1.0f, 2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f);
                    frustumCorners.push_back(pt / pt.w);
                }
            }
        }

        return frustumCorners;
	}

	std::vector<Vector4> Shadow::GetFrustumCornersWorldSpace(const Matrix4 &proj, const Matrix4 &view)
	{
		return GetFrustumCornersWorldSpace(proj * view);
	}

	Matrix4 Shadow::GetLightSpaceMatrix(const float nearPlane, const float farPlane)
	{
        auto viewportRect = Graphics::GetViewport();

        const auto proj = glm::perspective(glm::radians(camera->GetFieldOfView()), viewportRect.width / viewportRect.height, nearPlane, farPlane);
        
        const auto corners = GetFrustumCornersWorldSpace(proj, camera->GetViewMatrix());

        glm::vec3 center = glm::vec3(0, 0, 0);
        for (const auto& v : corners)
        {
            center += glm::vec3(v);
        }
        center /= corners.size();

        Vector3 lightDir = light->GetTransform()->GetForward();

        const auto lightView = glm::lookAt(center + lightDir, center, Vector3(0.0f, 1.0f, 0.0f));

        float minX = std::numeric_limits<float>::max();
        float maxX = std::numeric_limits<float>::min();
        float minY = std::numeric_limits<float>::max();
        float maxY = std::numeric_limits<float>::min();
        float minZ = std::numeric_limits<float>::max();
        float maxZ = std::numeric_limits<float>::min();

        for (const auto& v : corners)
        {
            const auto trf = lightView * v;
            minX = std::min(minX, trf.x);
            maxX = std::max(maxX, trf.x);
            minY = std::min(minY, trf.y);
            maxY = std::max(maxY, trf.y);
            minZ = std::min(minZ, trf.z);
            maxZ = std::max(maxZ, trf.z);
        }

        // Tune this parameter according to the scene
        constexpr float zMult = 1.0f;
        if (minZ < 0)
        {
            minZ *= zMult;
        }
        else
        {
            minZ /= zMult;
        }
        if (maxZ < 0)
        {
            maxZ /= zMult;
        }
        else
        {
            maxZ *= zMult;
        }

        const Matrix4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);
        return lightProjection * lightView;
	}

	std::vector<Matrix4> Shadow::GetLightSpaceMatrices()
	{
        std::vector<Matrix4> ret;

        float cameraNearPlane = camera->GetNearClippingPlane();
        float cameraFarPlane = camera->GetFarClippingPlane();

        for (size_t i = 0; i < shadowCascadeLevels.size() + 1; ++i)
        {
            if (i == 0)
            {
                ret.push_back(GetLightSpaceMatrix(cameraNearPlane, shadowCascadeLevels[i]));
            }
            else if (i < shadowCascadeLevels.size())
            {
                ret.push_back(GetLightSpaceMatrix(shadowCascadeLevels[i - 1], shadowCascadeLevels[i]));
            }
            else
            {
                ret.push_back(GetLightSpaceMatrix(shadowCascadeLevels[i - 1], cameraFarPlane));
            }
        }
        return ret;
	}

	void Shadow::UpdateUniformBuffer()
	{
		if(ubo == nullptr)
            ubo = Resources::FindUniformBuffer(Constants::GetString(ConstantString::UniformBufferShadow));

		if(ubo == nullptr)
			return;

		auto lightMatrices = GetLightSpaceMatrices();

		shadowData.farPlane = camera->GetFarClippingPlane();
		shadowData.shadowBias = 0.005f;
		shadowData.cascadeCount = shadowCascadeLevels.size();
		shadowData.enabled = enabled ? 1 : 0;

        for(size_t i = 0; i < lightMatrices.size(); i++)
        {
            shadowData.lightSpaceMatrices[i] = lightMatrices[i];
        }

        for(size_t i = 0; i < shadowCascadeLevels.size(); i++)
        {
            shadowData.cascadePlaneDistances[i].x = shadowCascadeLevels[i];
        }

        ubo->Bind();
        ubo->BufferSubData(0, sizeof(UniformShadowInfo), &shadowData);
        ubo->Unbind();
	}
}