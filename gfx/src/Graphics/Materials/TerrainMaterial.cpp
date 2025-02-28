#include "TerrainMaterial.hpp"
#include "../../Core/Resources.hpp"
#include "../../Core/Constants.hpp"
#include "../../External/glad/glad.h"

namespace GFX
{
	TerrainMaterial::TerrainMaterial() : Material()
	{
		shader = Resources::FindShader(Constants::GetString(ConstantString::ShaderTerrain));
		auto defaultTexture = Resources::FindTexture2D(Constants::GetString(ConstantString::TextureDefault));
		depthMap = Resources::FindTexture3D(Constants::GetString(ConstantString::TextureDepth));

		splatMap = defaultTexture;
		texture1 = defaultTexture;
		texture2 = defaultTexture;
		texture3 = defaultTexture;
		texture4 = defaultTexture;		
		diffuseColor = Color::White();
		ambientStrength = Constants::GetFloat(ConstantFloat::MaterialAmbientStrength);
		shininess = 16.0f;
		uvScale1 =Vector2(1, 1);
		uvScale2 =Vector2(1, 1);
		uvScale3 =Vector2(1, 1);
		uvScale4 =Vector2(1, 1);
		receiveShadows = true;

		if(shader != nullptr)
		{
			uModel = glGetUniformLocation(shader->GetId(), "uModel");
			uModelInverted = glGetUniformLocation(shader->GetId(), "uModelInverted");
			uMVP = glGetUniformLocation(shader->GetId(), "uMVP");
			uSplatMap = glGetUniformLocation(shader->GetId(), "uSplatMap");
			uTexture1 = glGetUniformLocation(shader->GetId(), "uTexture1");
			uTexture2 = glGetUniformLocation(shader->GetId(), "uTexture2");
			uTexture3 = glGetUniformLocation(shader->GetId(), "uTexture3");
			uTexture4 = glGetUniformLocation(shader->GetId(), "uTexture4");
			uUVScale1 = glGetUniformLocation(shader->GetId(), "uUVScale1");
			uUVScale2 = glGetUniformLocation(shader->GetId(), "uUVScale2");
			uUVScale3 = glGetUniformLocation(shader->GetId(), "uUVScale3");
			uUVScale4 = glGetUniformLocation(shader->GetId(), "uUVScale4");
			uAmbientStrength = glGetUniformLocation(shader->GetId(), "uAmbientStrength");
			uShininess = glGetUniformLocation(shader->GetId(), "uShininess");
			uDiffuseColor = glGetUniformLocation(shader->GetId(), "uDiffuseColor");
			uDepthMap = glGetUniformLocation(shader->GetId(), "uDepthMap");
			uReceiveShadows = glGetUniformLocation(shader->GetId(), "uReceiveShadows");
		}
	}

	void TerrainMaterial::Use(Transform *transform, Camera *camera)
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

		if(splatMap != nullptr)
		{
			splatMap->Bind(unit);
			shader->SetInt(uSplatMap, unit);
			unit++;
		}

		if(texture1 != nullptr)
		{
			texture1->Bind(unit);
			shader->SetInt(uTexture1, unit);
			unit++;
		}

		if(texture2 != nullptr)
		{
			texture2->Bind(unit);
			shader->SetInt(uTexture2, unit);
			unit++;
		}

		if(texture3 != nullptr)
		{
			texture3->Bind(unit);
			shader->SetInt(uTexture3, unit);
			unit++;
		}

		if(texture4 != nullptr)
		{
			texture4->Bind(unit);
			shader->SetInt(uTexture4, unit);
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
		shader->SetFloat2(uUVScale1, &uvScale1.x);
		shader->SetFloat2(uUVScale2, &uvScale2.x);
		shader->SetFloat2(uUVScale3, &uvScale3.x);
		shader->SetFloat2(uUVScale4, &uvScale4.x);
		shader->SetInt(uReceiveShadows, receiveShadows ? 1 : 0);
	}

    Texture2D *TerrainMaterial::GetSplatMap() const 
    {
        return splatMap;
    }

    void TerrainMaterial::SetSplatMap(Texture2D *value)
    {
        splatMap = value;
    }

    Texture2D *TerrainMaterial::GetTexture1() const 
    {
        return texture1;
    }

    void TerrainMaterial::SetTexture1(Texture2D *value)
    {
        texture1 = value;
    }

    Texture2D *TerrainMaterial::GetTexture2() const 
    {
        return texture2;
    }

    void TerrainMaterial::SetTexture2(Texture2D *value)
    {
        texture2 = value;
    }
    
    Texture2D *TerrainMaterial::GetTexture3() const 
    {
        return texture3;
    }

    void TerrainMaterial::SetTexture3(Texture2D *value)
    {
        texture3 = value;
    }
    
    Texture2D *TerrainMaterial::GetTexture4() const 
    {
        return texture4;
    }

    void TerrainMaterial::SetTexture4(Texture2D *value)
    {
        texture4 = value;
    }
    
    Vector2 TerrainMaterial::GetUvScale1() const 
    {
        return uvScale1;
    }

    void TerrainMaterial::SetUvScale1(const Vector2 &value)
    {
        uvScale1 = value;
    }
    
    Vector2 TerrainMaterial::GetUvScale2() const 
    {
        return uvScale2;
    }

    void TerrainMaterial::SetUvScale2(const Vector2 &value)
    {
        uvScale2 = value;
    }
    
    Vector2 TerrainMaterial::GetUvScale3() const 
    {
        return uvScale3;
    }

    void TerrainMaterial::SetUvScale3(const Vector2 &value)
    {
        uvScale3 = value;
    }

    Vector2 TerrainMaterial::GetUvScale4() const 
    {
        return uvScale4;
    }

    void TerrainMaterial::SetUvScale4(const Vector2 &value)
    {
        uvScale4 = value;
    }
    
    Color TerrainMaterial::GetDiffuseColor() const 
    {
        return diffuseColor;
    }

    void TerrainMaterial::SetDiffuseColor(const Color &value)
    {
        diffuseColor = value;
    }
    
    float TerrainMaterial::GetAmbientStrength() const 
    {
        return ambientStrength;
    }

    void TerrainMaterial::SetAmbientStrength(float value) 
    {
        ambientStrength = value;
    }
    
    float TerrainMaterial::GetShininess() const 
    {
        return shininess;
    }

    void TerrainMaterial::SetShininess(float value) 
    {
        shininess = value;
    }
    
    bool TerrainMaterial::GetReceiveShadows() const 
    {
        return receiveShadows;
    }

    void TerrainMaterial::SetReceiveShadows(bool value) 
    {
        receiveShadows = value;
    }
}