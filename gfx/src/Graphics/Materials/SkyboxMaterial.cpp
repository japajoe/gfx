#include "SkyboxMaterial.hpp"
#include "../../Core/Resources.hpp"
#include "../../Core/Constants.hpp"
#include "../../External/glad/glad.h"

namespace GFX
{
	SkyboxMaterial::SkyboxMaterial() : Material()
	{
		shader = Resources::FindShader(Constants::GetString(ConstantString::ShaderSkybox));
		diffuseTexture = Resources::FindTextureCubeMap(Constants::GetString(ConstantString::TextureDefaultCubeMap));
		diffuseColor = Color::White();

		if(shader != nullptr)
		{
			uModel = glGetUniformLocation(shader->GetId(), "uModel");
			uView = glGetUniformLocation(shader->GetId(), "uView");
			uProjection = glGetUniformLocation(shader->GetId(), "uProjection");
			uTexture = glGetUniformLocation(shader->GetId(), "uTexture");
			uDiffuseColor = glGetUniformLocation(shader->GetId(), "uDiffuseColor");	
		}
	}

	void SkyboxMaterial::Use(Transform *transform, Camera *camera)
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
		shader->SetFloat4(uDiffuseColor, &diffuseColor.r);

		int unit = 0;

		if(diffuseTexture != nullptr)
		{
			diffuseTexture->Bind(unit);
			shader->SetInt(uTexture, unit);
			unit++;
		}
	}

	TextureCubeMap *SkyboxMaterial::GetDiffuseTexture() const 
	{
		return diffuseTexture;
	}

	void SkyboxMaterial::SetDiffuseTexture(TextureCubeMap *value) 
	{
		diffuseTexture = value;
	}

	Color SkyboxMaterial::GetDiffuseColor() const 
	{
		return diffuseColor;
	}

	void SkyboxMaterial::SetDiffuseColor(const Color &value) 
	{
		diffuseColor = value;
	}
}