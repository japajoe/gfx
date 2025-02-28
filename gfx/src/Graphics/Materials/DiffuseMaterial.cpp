#include "DiffuseMaterial.hpp"
#include "../../Core/Resources.hpp"
#include "../../Core/Constants.hpp"
#include "../../External/glad/glad.h"

namespace GFX
{
	DiffuseMaterial::DiffuseMaterial() : Material()
	{
		shader = Resources::FindShader(Constants::GetString(ConstantString::ShaderDiffuse));
		diffuseTexture = Resources::FindTexture2D(Constants::GetString(ConstantString::TextureDefault));
		depthMap = Resources::FindTexture3D(Constants::GetString(ConstantString::TextureDepth));
		diffuseColor = Color::White();
		ambientStrength = Constants::GetFloat(ConstantFloat::MaterialAmbientStrength);
		shininess = 16.0f;
		uvScale = Vector2(1, 1);
		uvOffset = Vector2(0, 0);
		receiveShadows = true;

		if(shader != nullptr)
		{
			uModel = glGetUniformLocation(shader->GetId(), "uModel");
			uModelInverted = glGetUniformLocation(shader->GetId(), "uModelInverted");
			uMVP = glGetUniformLocation(shader->GetId(), "uMVP");
			uDiffuseTexture = glGetUniformLocation(shader->GetId(), "uDiffuseTexture");
			uDiffuseColor = glGetUniformLocation(shader->GetId(), "uDiffuseColor");
			uAmbientStrength = glGetUniformLocation(shader->GetId(), "uAmbientStrength");
			uShininess = glGetUniformLocation(shader->GetId(), "uShininess");
			uUVScale = glGetUniformLocation(shader->GetId(), "uUVScale");
			uUVOffset = glGetUniformLocation(shader->GetId(), "uUVOffset");
			uDepthMap = glGetUniformLocation(shader->GetId(), "uDepthMap");
			uReceiveShadows = glGetUniformLocation(shader->GetId(), "uReceiveShadows");
		}
	}

	void DiffuseMaterial::Use(Transform *transform, Camera *camera)
	{
		if(shader == nullptr || camera == nullptr || transform == nullptr)
			return;

		Matrix4 projection = camera->GetProjectionMatrix();
		Matrix4 view = camera->GetViewMatrix();
		Matrix4 model = transform->GetModelMatrix();
		Matrix4 MVP = projection * view * model;
        Matrix3 modelInverted = glm::inverse(glm::transpose(glm::mat3(model)));

		shader->Use();

		int unit = 0;

		if(diffuseTexture != nullptr)
		{
			diffuseTexture->Bind(unit);
			shader->SetInt(uDiffuseTexture, unit);
			unit++;
		}

		if(depthMap != nullptr)
		{
			depthMap->Bind(unit);
			shader->SetInt(uDepthMap, unit);
			unit++;
		}

		shader->SetMat4(uModel, glm::value_ptr(model));
		shader->SetMat3(uModelInverted, glm::value_ptr(modelInverted));
		shader->SetMat4(uMVP, glm::value_ptr(MVP));
		shader->SetFloat4(uDiffuseColor, &diffuseColor.r);
		shader->SetFloat(uAmbientStrength, ambientStrength);
		shader->SetFloat(uShininess, shininess);
		shader->SetFloat2(uUVScale, &uvScale.x);
		shader->SetFloat2(uUVOffset, &uvOffset.x);
		shader->SetInt(uReceiveShadows, receiveShadows ? 1 : 0);
	}

	Texture2D *DiffuseMaterial::GetDiffuseTexture() const 
	{
		return diffuseTexture;
	}

	void DiffuseMaterial::SetDiffuseTexture(Texture2D *value) 
	{
		diffuseTexture = value;
	}

	Color DiffuseMaterial::GetDiffuseColor() const 
	{
		return diffuseColor;
	}

	void DiffuseMaterial::SetDiffuseColor(const Color &value) 
	{
		diffuseColor = value;
	}

	float DiffuseMaterial::GetAmbientStrength() const 
	{
		return ambientStrength;
	}

	void DiffuseMaterial::SetAmbientStrength(float value) 
	{
		ambientStrength = value;
	}

	float DiffuseMaterial::GetShininess() const 
	{
		return shininess;
	}

	void DiffuseMaterial::SetShininess(float value) 
	{
		shininess = value;
	}

	Vector2 DiffuseMaterial::GetUVScale() const 
	{
		return uvScale;
	}

	void DiffuseMaterial::SetUVScale(const Vector2 &value) 
	{
		uvScale = value;
	}

	Vector2 DiffuseMaterial::GetUVOffset() const {
		return uvOffset;
	}

	void DiffuseMaterial::SetUVOffset(const Vector2 &value) 
	{
		uvOffset = value;
	}

	bool DiffuseMaterial::GetReceiveShadows() const 
	{
		return receiveShadows;
	}

	void DiffuseMaterial::SetReceiveShadows(bool value) 
	{
		receiveShadows = value;
	}
}