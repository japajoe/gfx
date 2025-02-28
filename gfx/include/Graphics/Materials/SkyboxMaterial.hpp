#ifndef GFX_SKYBOXMATERIAL_HPP
#define GFX_SKYBOXMATERIAL_HPP

#include "Material.hpp"

namespace GFX
{
	class SkyboxMaterial : public Material
	{
	private:
		int uModel;
		int uView;
		int uProjection;
		int uTexture;
		int uDiffuseColor;

        TextureCubeMap *diffuseTexture;
        Color diffuseColor;
	public:
		SkyboxMaterial();
		void Use(Transform *transform, Camera *camera) override;
		TextureCubeMap *GetDiffuseTexture() const;
		void SetDiffuseTexture(TextureCubeMap *value);
		Color GetDiffuseColor() const;
		void SetDiffuseColor(const Color &value);
	};
}

#endif