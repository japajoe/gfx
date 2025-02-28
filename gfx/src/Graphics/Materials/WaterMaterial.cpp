#include "WaterMaterial.hpp"
#include "../../Core/Resources.hpp"
#include "../../Core/Constants.hpp"
#include "../../External/glad/glad.h"
#include "../../External/glm/glm.hpp"
#include "../../System/Random.hpp"

namespace GFX
{
	WaterMaterial::WaterMaterial() : Material()
	{
		shader = Resources::FindShader(Constants::GetString(ConstantString::ShaderWater));
		diffuseTexture = Resources::FindTexture2D(Constants::GetString(ConstantString::TextureDefault));
		depthMap = Resources::FindTexture3D(Constants::GetString(ConstantString::TextureDepth));

		diffuseColor = Color::SkyBlue();
		ambientStrength = Constants::GetFloat(ConstantFloat::MaterialAmbientStrength);
		shininess = 16.0f;
		uvScale = Vector2(1, 1);
		uvOffset = Vector2(0, 0);
		receiveShadows = true;
		numWaves = 3;

		for(int i = 0; i < MAX_WAVES; i++)
		{
			float rx = Random::Range(-1.0f, 1.0f);
			float ry = Random::Range(-1.0f, 1.0f);
			waves[i].direction = Vector2(rx, ry);
			waves[i].amplitude = 1.0f;
			waves[i].steepness = 0.1f + (i * 0.1f);
			waves[i].waveLength = 0.2f + (i * 0.2f);
			waves[i].speed = 0.25f;
		}

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

			for (int i = 0; i < MAX_WAVES; ++i) 
			{
				std::string index = std::to_string(i);
				std::string direction = "uWaves[" + index + "].direction";
				std::string steepness = "uWaves[" + index + "].steepness";
				std::string waveLength = "uWaves[" + index + "].waveLength";
				std::string amplitude = "uWaves[" + index + "].amplitude";
				std::string speed = "uWaves[" + index + "].speed";

				uWaves[i].direction = glGetUniformLocation(shader->GetId(), direction.c_str());
				uWaves[i].steepness = glGetUniformLocation(shader->GetId(), steepness.c_str());
				uWaves[i].waveLength = glGetUniformLocation(shader->GetId(), waveLength.c_str());
				uWaves[i].amplitude = glGetUniformLocation(shader->GetId(), amplitude.c_str());
				uWaves[i].speed = glGetUniformLocation(shader->GetId(), speed.c_str());
			}

			uNumWaves = glGetUniformLocation(shader->GetId(), "uNumWaves");
		}
	}

	void WaterMaterial::Use(Transform *transform, Camera *camera)
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
		shader->SetInt(uNumWaves, numWaves);

		for(int i = 0; i < numWaves; i++)
		{
			shader->SetFloat2(uWaves[i].direction, &waves[i].direction.x);
			shader->SetFloat(uWaves[i].amplitude, waves[i].amplitude);
			shader->SetFloat(uWaves[i].steepness, waves[i].steepness);
			shader->SetFloat(uWaves[i].waveLength, waves[i].waveLength);
			shader->SetFloat(uWaves[i].speed, waves[i].speed);
		}
	}
	
    Texture2D *WaterMaterial::GetDiffuseTexture() const
    {
        return diffuseTexture;
    }

    void WaterMaterial::SetDiffuseTexture(Texture2D *texture)
    {
        diffuseTexture = texture;
    }

    Color WaterMaterial::GetDiffuseColor() const
    {
        return diffuseColor;
    }

    void WaterMaterial::SetDiffuseColor(const Color &color)
    {
        diffuseColor = color;
    }

    float WaterMaterial::GetAmbientStrength() const
    {
        return ambientStrength;
    }

    void WaterMaterial::SetAmbientStrength(float strength)
    {
        ambientStrength = strength;
    }

    float WaterMaterial::GetShininess() const
    {
        return shininess;
    }

    void WaterMaterial::SetShininess(float value)
    {
        shininess = value;
    }

    Vector2 WaterMaterial::GetUVScale() const
    {
        return uvScale;
    }

    void WaterMaterial::SetUVScale(const Vector2 &scale)
    {
        uvScale = scale;
    }

    Vector2 WaterMaterial::GetUVOffset() const
    {
        return uvOffset;
    }

    void WaterMaterial::SetUVOffset(const Vector2 &offset)
    {
        uvOffset = offset;
    }

    bool WaterMaterial::GetReceiveShadows() const
    {
        return receiveShadows;
    }

    void WaterMaterial::SetReceiveShadows(bool value)
    {
        receiveShadows = value;
    }

    int WaterMaterial::GetWaveCount() const
    {
        return numWaves;
    }

    void WaterMaterial::SetWaveCount(int count)
    {
        numWaves = glm::clamp(count, 1, MAX_WAVES);
    }

    int WaterMaterial::GetMaxWaveCount() const
    {
        return MAX_WAVES;
    }

    WaterMaterial::Wave *WaterMaterial::GetWaves()
    {
        return waves;
    }
}