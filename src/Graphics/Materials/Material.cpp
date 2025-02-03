#include "Material.hpp"

namespace GFX
{
	Material::Material()
	{
		shader = nullptr;
	}

	Shader *Material::GetShader() const
	{
		return shader;
	}

	void Material::Use(Transform *transform, Camera *camera)
	{

	}
}