#ifndef GFX_PROCEDURALSKYBOX2MATERIAL_HPP
#define GFX_PROCEDURALSKYBOX2MATERIAL_HPP

#include "Material.hpp"

namespace GFX
{
	class ProceduralSkybox2Material : public Material
	{
	private:
		int uModel;
		int uView;
		int uProjection;
		int uSunPosition;
		int uRayleigh;
		int uTurbidity;
		int uMieCoefficient;
		int uMieDirectionalG;
		int uUp;
		int uExposure;

		Vector3 sunPosition;
		float rayleigh;
		float turbidity;
		float mieCoefficient;
		Vector3 up;
		float mieDirectionalG;
		float exposure;
	public:
		ProceduralSkybox2Material();
		void Use(Transform *transform, Camera *camera) override;
		Vector3 GetSunPosition() const;
		void SetSunPosition(const Vector3 &position);
		void SetSunPositionFromLightDirection(const Vector3 &lightDirection);
		float GetRayleigh() const;
		void SetRayleigh(float value);
		float GetTurbidity() const;
		void SetTurbidity(float value);
		float GetMieCoefficient() const;
		void SetMieCoefficient(float value);
		Vector3 GetUp() const;
		void SetUp(const Vector3 &direction);
		float GetMieDirectionalG() const;
		void SetMieDirectionalG(float value);
		float GetExposure() const;
		void SetExposure(float value);
	};
}

#endif