#include "DepthMaterial.hpp"
#include "../../Core/Resources.hpp"
#include "../../Core/Constants.hpp"
#include "../../External/glad/glad.h"

namespace GFX
{
	DepthMaterial::DepthMaterial() : Material()
	{
		shader = Resources::FindShader(Constants::GetString(ConstantString::ShaderDepth));

		hasInstanceData = false;

		if(shader != nullptr)
		{
			uModel = glGetUniformLocation(shader->GetId(), "uModel");
			uHasInstanceData = glGetUniformLocation(shader->GetId(), "uHasInstanceData");			
		}
	}

	bool DepthMaterial::HasInstanceData() const
	{
		return hasInstanceData;
	}

	void DepthMaterial::SetHasInstanceData(bool hasInstanceData)
	{
		this->hasInstanceData = hasInstanceData;
	}

	void DepthMaterial::Use(Transform *transform, Camera *camera)
	{
		if(shader == nullptr || camera == nullptr || transform == nullptr)
			return;

		Matrix4 model = transform->GetModelMatrix();

		shader->Use();

		shader->SetMat4(uModel, glm::value_ptr(model));
		shader->SetInt(uHasInstanceData, hasInstanceData ? 1 : 0);
	}
}