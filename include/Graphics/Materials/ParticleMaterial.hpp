#ifndef GFX_PARTICLEMATERIAL_HPP
#define GFX_PARTICLEMATERIAL_HPP

#include "Material.hpp"

namespace GFX
{
	class ParticleMaterial : public Material
	{
	private:
        int uDiffuseTexture;
        int uUVOffset;
        int uUVScale;
        int uAmbientStrength;
        int uShininess;
        int uAlphaCutOff;
        int uDepthMap;
        int uReceiveShadows;

        Texture2D *diffuseTexture;
        Texture3D *depthMap;
        Vector2 uvScale;
        Vector2 uvOffset;
        float ambientStrength;
        float shininess;
        float alphaCutOff;
        bool receiveShadows;
	public:
		ParticleMaterial();
		void Use(Transform *transform, Camera *camera) override;
		Texture2D *GetDiffuseTexture() const;
		void SetDiffuseTexture(Texture2D *texture);
		Vector2 GetUVScale() const;
		void SetUVScale(const Vector2 &scale);
		Vector2 GetUVOffset() const;
		void SetUVOffset(const Vector2 &offset);
		float GetAlphaCutOff() const;
		void SetAlphaCutOff(float alphaCutOff);
	};
}

#endif