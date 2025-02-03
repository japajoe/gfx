#include "Texture.hpp"
#include "../External/glad/glad.h"

namespace GFX
{
	Texture::Texture()
	{
		id = 0;
	}

	uint32_t Texture::GetId() const 
	{ 
		return id; 
	}

	void Texture::ObjectLabel(const std::string &label)
	{
		glObjectLabel(GL_TEXTURE, id, -1, label.c_str());
	}
}