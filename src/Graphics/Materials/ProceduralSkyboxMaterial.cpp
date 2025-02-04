#include "ProceduralSkyboxMaterial.hpp"
#include "../../Core/Resources.hpp"
#include "../../Core/Constants.hpp"
#include "../../External/glad/glad.h"

namespace GFX
{
	ProceduralSkyboxMaterial::ProceduralSkyboxMaterial() : Material()
	{
		shader = Resources::FindShader(Constants::GetString(ConstantString::ShaderProceduralSkybox));
		cloudSpeed = 0.1f;
		cirrus = 0.677f;
		cumulus = 0.403f;
		rayleighCoefficient = 0.0075f;
		mieCoefficient = 0.0153f;
		scatteringDirection = 0.998f; // Mie scattering direction. Should be ALMOST 1.0f
		sunPosition = Vector3(-0.007f, 0.954f, -1.563f);

		if(shader != nullptr)
		{
			uModel = glGetUniformLocation(shader->GetId(), "uModel");
			uView = glGetUniformLocation(shader->GetId(), "uView");
			uProjection = glGetUniformLocation(shader->GetId(), "uProjection");
			uRayleighCoefficient = glGetUniformLocation(shader->GetId(), "uRayleighCoefficient");
			uMieCoefficient = glGetUniformLocation(shader->GetId(), "uMieCoefficient");
			uScatteringDirection = glGetUniformLocation(shader->GetId(), "uScatteringDirection");
			uCloudSpeed = glGetUniformLocation(shader->GetId(), "uCloudSpeed");
			uCirrus = glGetUniformLocation(shader->GetId(), "uCirrus");
			uCumulus = glGetUniformLocation(shader->GetId(), "uCumulus");
			uSunPosition = glGetUniformLocation(shader->GetId(), "uSunPosition");
		}
	}
	
	void ProceduralSkyboxMaterial::Use(Transform *transform, Camera *camera)
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

		shader->SetFloat(uCloudSpeed, cloudSpeed);
		shader->SetFloat(uCirrus, cirrus);
		shader->SetFloat(uCumulus, cumulus);
		shader->SetFloat(uRayleighCoefficient, rayleighCoefficient);
		shader->SetFloat(uMieCoefficient, mieCoefficient);
		shader->SetFloat(uScatteringDirection, scatteringDirection);
		shader->SetFloat3(uSunPosition, &sunPosition.x);
	}

	float ProceduralSkyboxMaterial::GetCloudSpeed() const 
	{
		return cloudSpeed;
	}

	void ProceduralSkyboxMaterial::SetCloudSpeed(float value) 
	{
		cloudSpeed = value;
	}

	float ProceduralSkyboxMaterial::GetCirrus() const 
	{
		return cirrus;
	}

	void ProceduralSkyboxMaterial::SetCirrus(float value) 
	{
		cirrus = value;
	}

	float ProceduralSkyboxMaterial::GetCumulus() const 
	{
		return cumulus;
	}

	void ProceduralSkyboxMaterial::SetCumulus(float value) 
	{
		cumulus = value;
	}

	float ProceduralSkyboxMaterial::GetRayleighCoefficient() const 
	{
		return rayleighCoefficient;
	}

	void ProceduralSkyboxMaterial::SetRayleighCoefficient(float value) 
	{
		rayleighCoefficient = value;
	}

	float ProceduralSkyboxMaterial::GetMieCoefficient() const 
	{
		return mieCoefficient;
	}

	void ProceduralSkyboxMaterial::SetMieCoefficient(float value) 
	{
		mieCoefficient = value;
	}

	float ProceduralSkyboxMaterial::GetScatteringDirection() const 
	{
		return scatteringDirection;
	}

	void ProceduralSkyboxMaterial::SetScatteringDirection(float value) 
	{
		scatteringDirection = value;
	}

	Vector3 ProceduralSkyboxMaterial::GetSunPosition() const 
	{
		return sunPosition;
	}

	void ProceduralSkyboxMaterial::SetSunPosition(const Vector3 &value) 
	{
		sunPosition = value;
	}
}