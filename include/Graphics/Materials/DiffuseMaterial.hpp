#ifndef GFX_DIFFUSEMATERIAL_HPP
#define GFX_DIFFUSEMATERIAL_HPP

#include "Material.hpp"
#include "../Texture2D.hpp"
#include "../Texture3D.hpp"
#include "../Color.hpp"
#include "../../System/Numerics/Vector2.hpp"
#include "../../System/Numerics/Vector3.hpp"
#include "../../System/Numerics/Vector4.hpp"
#include "../../System/Numerics/Matrix3.hpp"
#include "../../System/Numerics/Matrix4.hpp"

namespace GFX
{
	class DiffuseMaterial : public Material
	{
	private:
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

        Texture2D *diffuseTexture;
        Texture3D *depthMap;
        Color diffuseColor;
        float ambientStrength;
        float shininess;
        Vector2 uvScale;
        Vector2 uvOffset;
        bool receiveShadows;
	public:
		DiffuseMaterial();
		void Use(Transform *transform, Camera *camera) override;
		Texture2D *GetDiffuseTexture() const;
		void SetDiffuseTexture(Texture2D *value);
		Color GetDiffuseColor() const;
		void SetDiffuseColor(const Color &value);
		float GetAmbientStrength() const;
		void SetAmbientStrength(float value);
		float GetShininess() const;
		void SetShininess(float value);
		Vector2 GetUVScale() const;
		void SetUVScale(const Vector2 &value);
		Vector2 GetUVOffset() const;
		void SetUVOffset(const Vector2 &value);
		bool GetReceiveShadows() const;
		void SetReceiveShadows(bool value);
	};
}

#endif