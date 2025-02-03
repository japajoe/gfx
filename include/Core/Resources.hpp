#ifndef GFX_RESOURCES_HPP
#define GFX_RESOURCES_HPP

#include "../Graphics/Buffers/UniformBufferObject.hpp"
#include "../Graphics/Shader.hpp"
#include "../Graphics/Texture2D.hpp"
#include "../Graphics/Font.hpp"
#include <unordered_map>
#include <string>

namespace GFX
{
	class Resources
	{
	private:
		static std::unordered_map<std::string,UniformBufferObject> uniformBuffers;
		static std::unordered_map<std::string,Font> fonts;
		static std::unordered_map<std::string,Shader> shaders;
		static std::unordered_map<std::string,Texture2D> textures2D;
	public:
		static UniformBufferObject *AddUniformBuffer(const std::string &name, const UniformBufferObject &ubo);
		static UniformBufferObject *FindUniformBuffer(const std::string &name);

		static Font *AddFont(const std::string &name, const Font &font);
		static Font *FindFont(const std::string &name);

		static Shader *AddShader(const std::string &name, const Shader &shader);
		static Shader *FindShader(const std::string &name);

		static Texture2D *AddTexture2D(const std::string &name, const Texture2D &texture);
		static Texture2D *FindTexture2D(const std::string &name);
	};
}

#endif