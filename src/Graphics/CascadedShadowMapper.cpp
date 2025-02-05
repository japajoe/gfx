#include "CascadedShadowMapper.hpp"
#include "../External/glad/glad.h"

namespace GFX
{
	CascadedShadowMapper::CascadedShadowMapper(int numCascades, int shadowMapSize)
		: numCascades(numCascades), shadowMapSize(shadowMapSize) {}

	CascadedShadowMapper::~CascadedShadowMapper()
	{
		glDeleteFramebuffers(1, &shadowFBO);
		glDeleteTextures(1, &shadowMapArray);
	}

	void CascadedShadowMapper::Init()
	{
		// Create shadow map array texture
		glGenTextures(1, &shadowMapArray);
		glBindTexture(GL_TEXTURE_2D_ARRAY, shadowMapArray);
		glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F,
					 shadowMapSize, shadowMapSize, numCascades,
					 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);

		// Create FBO
		glGenFramebuffers(1, &shadowFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowMapArray, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void CascadedShadowMapper::UpdateCascades(const glm::mat4 &cameraView, float fov,
											  float aspect, float cameraNear,
											  float cameraFar, const glm::vec3 &lightDir)
	{
		// Calculate split distances
		cascadeSplits.resize(numCascades);
		for (int i = 0; i < numCascades; ++i)
		{
			float p = (i + 1) / static_cast<float>(numCascades);
			float logSplit = cameraNear * pow(cameraFar / cameraNear, p);
			float uniformSplit = cameraNear + (cameraFar - cameraNear) * p;
			cascadeSplits[i] = lambda * logSplit + (1 - lambda) * uniformSplit;
		}

		CalculateOrthoProjs(cameraView, fov, aspect, cameraNear, cameraFar, lightDir);
	}

	void CascadedShadowMapper::BeginRenderCascade(int cascadeIndex)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
		glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
								  shadowMapArray, 0, cascadeIndex);
		glViewport(0, 0, shadowMapSize, shadowMapSize);
		glClear(GL_DEPTH_BUFFER_BIT);
	}

	void CascadedShadowMapper::CalculateOrthoProjs(const glm::mat4 &cameraView, float fov,
												   float aspect, float cameraNear,
												   float cameraFar, const glm::vec3 &lightDir)
	{
		lightVPs.resize(numCascades);
		float lastSplit = cameraNear;

		for (int i = 0; i < numCascades; ++i)
		{
			float split = cascadeSplits[i];

			// Calculate frustum corners in world space
			glm::mat4 invViewProj = glm::inverse(cameraView *
												 glm::perspective(fov, aspect, lastSplit, split));

			std::vector<glm::vec3> frustumCorners;
			CalculateFrustumCorners(invViewProj, frustumCorners);

			// Calculate light space AABB
			glm::vec3 minAABB(FLT_MAX);
			glm::vec3 maxAABB(-FLT_MAX);
			glm::mat4 lightView = glm::lookAt(glm::vec3(0.0f), lightDir, glm::vec3(0, 1, 0));

			for (const auto &corner : frustumCorners)
			{
				glm::vec4 lightSpaceCorner = lightView * glm::vec4(corner, 1.0f);
				minAABB = glm::min(minAABB, glm::vec3(lightSpaceCorner));
				maxAABB = glm::max(maxAABB, glm::vec3(lightSpaceCorner));
			}

			// Snap to texel grid to prevent flickering
			float worldUnitsPerTexel = (maxAABB.x - minAABB.x) / shadowMapSize;
			minAABB.x = floor(minAABB.x / worldUnitsPerTexel) * worldUnitsPerTexel;
			minAABB.y = floor(minAABB.y / worldUnitsPerTexel) * worldUnitsPerTexel;
			maxAABB.x = floor(maxAABB.x / worldUnitsPerTexel) * worldUnitsPerTexel;
			maxAABB.y = floor(maxAABB.y / worldUnitsPerTexel) * worldUnitsPerTexel;

			glm::mat4 lightProj = glm::ortho(
				minAABB.x, maxAABB.x,
				minAABB.y, maxAABB.y,
				minAABB.z - 1000.0f, // Expand Z bounds slightly
				maxAABB.z + 1000.0f);

			lightVPs[i] = lightProj * lightView;
			lastSplit = split;
		}
	}

	void CascadedShadowMapper::CalculateFrustumCorners(const glm::mat4 &inverseViewProj,
													   std::vector<glm::vec3> &outCorners) const
	{
		outCorners.resize(8);
		const glm::vec3 frustumCorners[8] = {
			{-1, -1, -1}, {1, -1, -1}, {1, 1, -1}, {-1, 1, -1}, {-1, -1, 1}, {1, -1, 1}, {1, 1, 1}, {-1, 1, 1}};

		for (int i = 0; i < 8; ++i)
		{
			glm::vec4 corner = inverseViewProj * glm::vec4(frustumCorners[i], 1.0f);
			outCorners[i] = glm::vec3(corner) / corner.w;
		}
	}

	const glm::mat4 &CascadedShadowMapper::GetLightVP(int cascadeIndex) const
	{
		return lightVPs[cascadeIndex];
	}
}