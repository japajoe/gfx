#ifndef GFX_SHADOW_HPP
#define GFX_SHADOW_HPP

#include "../External/glm/glm.hpp"
#include "../System/Numerics/Vector3.hpp"
#include "../System/Numerics/Vector4.hpp"
#include "../System/Numerics/Matrix4.hpp"
#include <cstdint>
#include <vector>

namespace GFX
{
	class Texture3D;
	class UniformBufferObject;
	class Camera;
	class Light;

    struct UniformShadowInfo
    {
        int cascadeCount;
        float shadowBias;
        float farPlane;
        int enabled;
        Matrix4 lightSpaceMatrices[16];
        Vector4 cascadePlaneDistances[16];
    };

	class Shadow
	{
	private:
        uint32_t lightFBO;
        Texture3D *depthMap;
        UniformBufferObject *ubo;
        UniformShadowInfo shadowData;
        std::vector<float> shadowCascadeLevels;
		Camera *camera;
		Light *light;
        static bool enabled;
		std::vector<Vector4> GetFrustumCornersWorldSpace(const Matrix4 &projview);
		std::vector<Vector4> GetFrustumCornersWorldSpace(const Matrix4 &proj, const Matrix4 &view);
		Matrix4 GetLightSpaceMatrix(const float nearPlane, const float farPlane);
		std::vector<Matrix4> GetLightSpaceMatrices();
	public:
		Shadow();
		void Generate();
		void Bind();
		void Unbind();
		void UpdateUniformBuffer();
		static bool IsEnabled();
		static void SetEnabled(bool enabled);
	};
};

#endif