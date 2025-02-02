#include "Texture.hpp"

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
}