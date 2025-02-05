#ifndef GFX_CASCADEDSHADOWMAPPER_HPP
#define GFX_CASCADEDSHADOWMAPPER_HPP

#include <vector>
#include "../External/glm/glm.hpp"

namespace GFX
{
	class CascadedShadowMapper
	{
	private:
		void CalculateFrustumCorners(const glm::mat4 &inverseViewProj,
									 std::vector<glm::vec3> &outCorners) const;
		void CalculateOrthoProjs(const glm::mat4 &cameraView, float fov, float aspect,
								 float cameraNear, float cameraFar, const glm::vec3 &lightDir);

		int numCascades;
		int shadowMapSize;
		unsigned int shadowFBO;
		unsigned int shadowMapArray;

		std::vector<float> cascadeSplits;
		std::vector<glm::mat4> lightVPs;
		float lambda = 0.5f;

	public:
		CascadedShadowMapper(int numCascades = 4, int shadowMapSize = 2048);
		~CascadedShadowMapper();

		void Init();
		void UpdateCascades(const glm::mat4 &cameraView, float fov, float aspect,
							float cameraNear, float cameraFar, const glm::vec3 &lightDir);
		void BeginRenderCascade(int cascadeIndex);
		const glm::mat4 &GetLightVP(int cascadeIndex) const;

		// Getters for shader uniforms
		unsigned int GetShadowMapArray() const { return shadowMapArray; }
		const std::vector<float> &GetSplitDepths() const { return cascadeSplits; }
		const std::vector<glm::mat4> &GetLightVPs() const { return lightVPs; }
		int GetNumCascades() const { return numCascades; }
	};
}

#endif