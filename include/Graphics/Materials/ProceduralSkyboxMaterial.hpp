#ifndef GFX_PROCEDURALSKYBOXMATERIAL_HPP
#define GFX_PROCEDURALSKYBOXMATERIAL_HPP

#include "Material.hpp"

namespace GFX
{
	class ProceduralSkyboxMaterial : public Material
	{
	private:
        int uModel;
        int uView;
        int uProjection;
        int uRayleighCoefficient;
        int uMieCoefficient;
        int uScatteringDirection;
        int uCloudSpeed;
        int uCirrus;
        int uCumulus;
        int uSunPosition;

        float cloudSpeed;
        float cirrus;
        float cumulus;
        float rayleighCoefficient;
        float mieCoefficient;
        float scatteringDirection;
        Vector3 sunPosition;
	public:
		ProceduralSkyboxMaterial();
		void Use(Transform *transform, Camera *camera) override;
		float GetCloudSpeed() const;
		void SetCloudSpeed(float value);
		float GetCirrus() const;
		void SetCirrus(float value);
		float GetCumulus() const;
		void SetCumulus(float value);
		float GetRayleighCoefficient() const;
		void SetRayleighCoefficient(float value);
		float GetMieCoefficient() const;
		void SetMieCoefficient(float value);
		float GetScatteringDirection() const;
		void SetScatteringDirection(float value);
		Vector3 GetSunPosition() const;
		void SetSunPosition(const Vector3 &value);
	};
}

#endif