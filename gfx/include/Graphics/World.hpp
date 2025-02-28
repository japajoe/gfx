#ifndef GFX_WORLD_HPP
#define GFX_WORLD_HPP

#include "Color.hpp"

namespace GFX
{
    struct UniformWorldInfo
    {
        Color fogColor;     //16 don't use vec3 because the alignment causes issues
        float fogDensity;   //4
        float fogGradient;  //4
        int fogEnabled;     //4
        float time;         //4
        float padding1;     //4
        float padding2;     //4
        float padding3;     //4
        float padding4;     //4
    };

	class UniformBufferObject;

	class World
	{
	friend class Graphics;
	private:
        static Color fogColor;
        static float fogDensity;
        static float fogGradient;
        static bool fogEnabled;
        static UniformBufferObject *ubo;
		static void UpdateUniformBuffer();
	public:
		static Color GetFogColor();
		static void SetFogColor(const Color &color);
		static float GetFogDensity();
		static void SetFogDensity(float density);
		static float GetFogGradient();
		static void SetFogGradient(float gradient);
		static bool GetFogEnabled();
		static void SetFogEnabled(bool enabled);
	};
}

#endif