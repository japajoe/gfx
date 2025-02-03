#include "Resources.hpp"

namespace GFX
{
	std::unordered_map<std::string,UniformBufferObject> Resources::uniformBuffers;
	std::unordered_map<std::string,Font> Resources::fonts;
	std::unordered_map<std::string,Shader> Resources::shaders;
	std::unordered_map<std::string,Texture2D> Resources::textures2D;

	UniformBufferObject *Resources::AddUniformBuffer(const std::string &name, const UniformBufferObject &ubo)
	{
		if(uniformBuffers.contains(name))
			return nullptr;
		uniformBuffers[name] = ubo;
		return &uniformBuffers[name];
	}

	UniformBufferObject *Resources::FindUniformBuffer(const std::string &name)
	{
		if(!uniformBuffers.contains(name))
			return nullptr;
		return &uniformBuffers[name];
	}

	Font *Resources::AddFont(const std::string &name, const Font &font)
	{
		if(fonts.contains(name))
			return nullptr;
		fonts[name] = font;
		
		return &fonts[name];
	}

	Font *Resources::FindFont(const std::string &name)
	{
		if(!fonts.contains(name))
			return nullptr;
		return &fonts[name];
	}

	Shader *Resources::AddShader(const std::string &name, const Shader &shader)
	{
		if(shaders.contains(name))
			return nullptr;
		shaders[name] = shader;
		return &shaders[name];
	}

	Shader *Resources::FindShader(const std::string &name)
	{
		if(!shaders.contains(name))
			return nullptr;
		return &shaders[name];
	}

	Texture2D *Resources::AddTexture2D(const std::string &name, const Texture2D &texture)
	{
		if(textures2D.contains(name))
			return nullptr;
		textures2D[name] = texture;
		return &textures2D[name];
	}

	Texture2D *Resources::FindTexture2D(const std::string &name)
	{
		if(!textures2D.contains(name))
			return nullptr;
		return &textures2D[name];
	}
}