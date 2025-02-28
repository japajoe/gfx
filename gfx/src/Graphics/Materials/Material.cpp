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

	void Material::SetName(const std::string &name)
	{
		this->name = name;
	}

	std::string Material::GetName() const
	{
		return name;
	}

	void Material::Use(Transform *transform, Camera *camera)
	{

	}
}