#ifndef GFX_WATERMATERIAL_HPP
#define GFX_WATERMATERIAL_HPP

#include "Material.hpp"

namespace GFX
{
	class WaterMaterial : public Material
	{
	public:
		struct Wave
		{
			Vector2 direction;
			float steepness;
			float waveLength;
			float amplitude;
			float speed;
		};
	private:
        struct WaveUniform
        {
            int direction;
            int steepness;
            int waveLength;
            int amplitude;
            int speed;
        };

        int uModel;
        int uModelInverted;
        int uMVP;
        int uDiffuseTexture;
        int uDiffuseColor;
        int uAmbientStrength;
        int uShininess;
        int uUVScale;
        int uUVOffset;
        int uDepthMap;
        int uReceiveShadows;
        static constexpr int MAX_WAVES = 10;
        WaveUniform uWaves[MAX_WAVES];
        int uNumWaves;

        Texture2D *diffuseTexture;
        Texture3D *depthMap;
        Color diffuseColor;
        float ambientStrength;
        float shininess;
        Vector2 uvScale;
        Vector2 uvOffset;
        bool receiveShadows;
        Wave waves[MAX_WAVES];
        int numWaves;
	public:
		WaterMaterial();
		void Use(Transform *transform, Camera *camera) override;
		Texture2D *GetDiffuseTexture() const;
		void SetDiffuseTexture(Texture2D *texture);
		Color GetDiffuseColor() const;
		void SetDiffuseColor(const Color &color);
		float GetAmbientStrength() const;
		void SetAmbientStrength(float strength);
		float GetShininess() const;
		void SetShininess(float value);
		Vector2 GetUVScale() const;
		void SetUVScale(const Vector2 &scale);
		Vector2 GetUVOffset() const;
		void SetUVOffset(const Vector2 &offset);
		bool GetReceiveShadows() const;
		void SetReceiveShadows(bool value);
		int GetWaveCount() const;
		void SetWaveCount(int count);
		int GetMaxWaveCount() const;
		Wave *GetWaves();
	};
}

#endif