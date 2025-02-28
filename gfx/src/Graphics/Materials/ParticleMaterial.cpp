#include "ParticleMaterial.hpp"
#include "../../Core/Resources.hpp"
#include "../../Core/Constants.hpp"

namespace GFX
{
	ParticleMaterial::ParticleMaterial() : Material()
	{
		shader = Resources::FindShader(Constants::GetString(ConstantString::ShaderParticle));

		diffuseTexture = Resources::FindTexture2D(Constants::GetString(ConstantString::TextureDefault));
		depthMap = Resources::FindTexture3D(Constants::GetString(ConstantString::TextureDepth));
		uvScale = Vector2(1, 1);
		uvOffset = Vector2(0, 0);
		alphaCutOff = 0.5f;
		ambientStrength = Constants::GetFloat(ConstantFloat::MaterialAmbientStrength);
		shininess = 16.0f;
		receiveShadows = false;

		if(shader != nullptr)
		{
			uDiffuseTexture = glGetUniformLocation(shader->GetId(), "uDiffuseTexture");
			uUVOffset = glGetUniformLocation(shader->GetId(), "uUVOffset");
			uUVScale = glGetUniformLocation(shader->GetId(), "uUVScale");
			uAmbientStrength = glGetUniformLocation(shader->GetId(), "uAmbientStrength");
			uShininess = glGetUniformLocation(shader->GetId(), "uShininess");
			uAlphaCutOff = glGetUniformLocation(shader->GetId(), "uAlphaCutOff");
			uDepthMap = glGetUniformLocation(shader->GetId(), "uDepthMap");
			uReceiveShadows = glGetUniformLocation(shader->GetId(), "uReceiveShadows");
		}
	}

	void ParticleMaterial::Use(Transform *transform, Camera *camera)
	{
		if(!shader || !camera || !transform)
			return;

		shader->Use();

		int unit = 0;

		if(diffuseTexture)
		{
			diffuseTexture->Bind(unit);
			shader->SetInt(uDiffuseTexture, unit);
			unit++;
		}

		if(depthMap)
		{
			depthMap->Bind(unit);
			shader->SetInt(uDepthMap, unit);
			unit++;
		}

		shader->SetFloat2(uUVScale, glm::value_ptr(uvScale));
		shader->SetFloat2(uUVOffset, glm::value_ptr(uvOffset));
		shader->SetFloat(uAlphaCutOff, alphaCutOff);
		shader->SetFloat(uAmbientStrength, ambientStrength);
		shader->SetFloat(uShininess, shininess);
		shader->SetInt(uReceiveShadows, receiveShadows ? 1 : 0);
	}

	Texture2D *ParticleMaterial::GetDiffuseTexture() const
	{
		return diffuseTexture;
	}

	void ParticleMaterial::SetDiffuseTexture(Texture2D *texture)
	{
		diffuseTexture = texture;
	}

	Vector2 ParticleMaterial::GetUVScale() const
	{
		return uvScale;
	}

	void ParticleMaterial::SetUVScale(const Vector2 &scale)
	{
		uvScale = scale;
	}

	Vector2 ParticleMaterial::GetUVOffset() const
	{
		return uvOffset;
	}

	void ParticleMaterial::SetUVOffset(const Vector2 &offset)
	{
		uvOffset = offset;
	}

	float ParticleMaterial::GetAlphaCutOff() const
	{
		return alphaCutOff;
	}

	void ParticleMaterial::SetAlphaCutOff(float alphaCutOff)
	{
		this->alphaCutOff = alphaCutOff;
	}
}