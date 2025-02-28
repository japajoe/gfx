#ifndef GFX_TERRAINMATERIAL_HPP
#define GFX_TERRAINMATERIAL_HPP

#include "Material.hpp"

namespace GFX
{
	class TerrainMaterial : public Material
	{
	private:
        int uModel;
        int uModelInverted;
        int uMVP;
        int uSplatMap;
        int uTexture1;
        int uTexture2;
        int uTexture3;
        int uTexture4;
        int uUVScale1;
        int uUVScale2;
        int uUVScale3;
        int uUVScale4;
        int uAmbientStrength;
        int uShininess;
        int uDiffuseColor;
        int uDepthMap;
        int uReceiveShadows;

        Texture2D *splatMap;
        Texture2D *texture1;
        Texture2D *texture2;
        Texture2D *texture3;
        Texture2D *texture4;    
        Texture3D *depthMap;    
        Vector2 uvScale1;
        Vector2 uvScale2;
        Vector2 uvScale3;
        Vector2 uvScale4;
        Color diffuseColor;
        float ambientStrength;
        float shininess;
        bool receiveShadows;
	public:
		TerrainMaterial();
		void Use(Transform *transform, Camera *camera) override;
		Texture2D *GetSplatMap() const;
		void SetSplatMap(Texture2D *value);
		Texture2D *GetTexture1() const;
		void SetTexture1(Texture2D *value);
		Texture2D *GetTexture2() const;
		void SetTexture2(Texture2D *value);
		Texture2D *GetTexture3() const;
		void SetTexture3(Texture2D *value);
		Texture2D *GetTexture4() const;
		void SetTexture4(Texture2D *value);
		Vector2 GetUvScale1() const;
		void SetUvScale1(const Vector2 &value);
		Vector2 GetUvScale2() const;
		void SetUvScale2(const Vector2 &value);
		Vector2 GetUvScale3() const;
		void SetUvScale3(const Vector2 &value);
		Vector2 GetUvScale4() const;
		void SetUvScale4(const Vector2 &value);
		Color GetDiffuseColor() const;
		void SetDiffuseColor(const Color &value);
		float GetAmbientStrength() const;
		void SetAmbientStrength(float value);
		float GetShininess() const;
		void SetShininess(float value);
		bool GetReceiveShadows() const;
		void SetReceiveShadows(bool value);
	};
}

#endif