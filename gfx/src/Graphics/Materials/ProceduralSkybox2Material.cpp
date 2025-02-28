#include "ProceduralSkybox2Material.hpp"
#include "../../Core/Resources.hpp"
#include "../../Core/Constants.hpp"
#include "../../External/glad/glad.h"

namespace GFX
{
	ProceduralSkybox2Material::ProceduralSkybox2Material() : Material()
	{
		shader = Resources::FindShader(Constants::GetString(ConstantString::ShaderProceduralSkybox2));

		const float elevation = 2.0f;
		const float azimuth = 180.0f;
		const float phi = glm::radians(90.0f - elevation);
		const float theta = glm::radians(azimuth);

		sunPosition = Vector3f::FromSphericalCoordinates(1.0f, phi, theta);
		rayleigh = 3.0f;
		turbidity = 10.0f;
		mieCoefficient = 0.005f;
		up = Vector3(0, 1, 0);
		mieDirectionalG = 0.7f;
		exposure = 1.0f;

		if(shader != nullptr)
		{
			uModel = glGetUniformLocation(shader->GetId(), "uModel");
			uView = glGetUniformLocation(shader->GetId(), "uView");
			uProjection = glGetUniformLocation(shader->GetId(), "uProjection");
			uSunPosition = glGetUniformLocation(shader->GetId(), "uSunPosition");
			uRayleigh = glGetUniformLocation(shader->GetId(), "uRayleigh");
			uTurbidity = glGetUniformLocation(shader->GetId(), "uTurbidity");
			uMieCoefficient = glGetUniformLocation(shader->GetId(), "uMieCoefficient");
			uMieDirectionalG = glGetUniformLocation(shader->GetId(), "uMieDirectionalG");
			uUp = glGetUniformLocation(shader->GetId(), "uUp");
			uExposure = glGetUniformLocation(shader->GetId(), "uExposure");
		}
	}
	
	void ProceduralSkybox2Material::Use(Transform *transform, Camera *camera)
	{
		if(shader == nullptr || camera == nullptr || transform == nullptr)
			return;

		Matrix4 projection = camera->GetProjectionMatrix();
		Matrix4 view = camera->GetViewMatrix();
		Matrix4 model = transform->GetModelMatrix();

		shader->Use();

		shader->SetMat4(uModel, glm::value_ptr(model));
		shader->SetMat4(uView, glm::value_ptr(view));
		shader->SetMat4(uProjection, glm::value_ptr(projection));

		shader->SetFloat3(uSunPosition, glm::value_ptr(sunPosition));
		shader->SetFloat(uRayleigh, rayleigh);
		shader->SetFloat(uTurbidity, turbidity);
		shader->SetFloat(uMieCoefficient, mieCoefficient);
		shader->SetFloat(uMieDirectionalG, mieDirectionalG);
		shader->SetFloat3(uUp, glm::value_ptr(up));
		shader->SetFloat(uExposure, exposure);
	}

	Vector3 ProceduralSkybox2Material::GetSunPosition() const
	{
		return sunPosition;
	}

	void ProceduralSkybox2Material::SetSunPosition(const Vector3 &position)
	{
		sunPosition = position;
	}

	void ProceduralSkybox2Material::SetSunPositionFromLightDirection(const Vector3 &lightDirection)
	{
		Vector3 direction = glm::normalize(lightDirection); // Normalize the light direction

		// Calculate elevation (phi) and azimuth (theta)
		float elevation = glm::degrees(asin(direction.y)); // Elevation in degrees
		float azimuth = glm::degrees(atan2(direction.z, direction.x)); // Azimuth in degrees

		// Optionally, n adjust the azimuth to be in the range [0, 360)
		if (azimuth < 0.0f)
			azimuth += 360.0f;

		const float phi = glm::radians(90.0f - elevation);
		const float theta = glm::radians(90.0f - azimuth);
		sunPosition = Vector3f::FromSphericalCoordinates(1.0f, phi, theta);
	}

	float ProceduralSkybox2Material::GetRayleigh() const
	{
		return rayleigh;
	}

	void ProceduralSkybox2Material::SetRayleigh(float value)
	{
		rayleigh = value;
	}

	float ProceduralSkybox2Material::GetTurbidity() const
	{
		return turbidity;
	}

	void ProceduralSkybox2Material::SetTurbidity(float value)
	{
		turbidity = value;
	}

	float ProceduralSkybox2Material::GetMieCoefficient() const
	{
		return mieCoefficient;
	}

	void ProceduralSkybox2Material::SetMieCoefficient(float value)
	{
		mieCoefficient = value;
	}

	Vector3 ProceduralSkybox2Material::GetUp() const
	{
		return up;
	}

	void ProceduralSkybox2Material::SetUp(const Vector3 &direction)
	{
		up = direction;
	}

	float ProceduralSkybox2Material::GetMieDirectionalG() const
	{
		return mieDirectionalG;
	}

	void ProceduralSkybox2Material::SetMieDirectionalG(float value)
	{
		mieDirectionalG = value;
	}

	float ProceduralSkybox2Material::GetExposure() const
	{
		return exposure;
	}

	void ProceduralSkybox2Material::SetExposure(float value)
	{
		exposure = value;
	}
}