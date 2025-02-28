#include "Texture3D.hpp"
#include "Texture2D.hpp"
#include "../External/glad/glad.h"
#include <utility>

namespace GFX
{
	Texture3D::Texture3D() : Texture()
	{
		width = 0;
		height = 0;
		depth = 0;
	}

    Texture3D::Texture3D(uint32_t width, uint32_t height, uint32_t depth) : Texture()
    {
        this->width = width;
        this->height = height;
        this->depth = depth;

        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D_ARRAY, id);
        glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F, width, height, depth, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        constexpr float bordercolor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, bordercolor);
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    }

	Texture3D::Texture3D(const Texture3D &other)
	{
		this->id = other.id;
		this->width = other.width;
		this->height = other.height;
		this->depth = other.depth;
	}

	Texture3D::Texture3D(Texture3D &&other) noexcept
	{
		this->id = std::exchange(other.id, 0);
		this->width = std::exchange(other.width, 0);
		this->height = std::exchange(other.height, 0);
		this->depth = std::exchange(other.depth, 0);
	}

	Texture3D& Texture3D::operator=(const Texture3D &other)
	{
		if(this != &other)
		{
			this->id = other.id;
			this->width = other.width;
			this->height = other.height;
			this->depth = other.depth;
		}
		return *this;
	}

	Texture3D& Texture3D::operator=(Texture3D &&other) noexcept
	{
		if(this != &other)
		{
			this->id = std::exchange(other.id, 0);
			this->width = std::exchange(other.width, 0);
			this->height = std::exchange(other.height, 0);
			this->depth = std::exchange(other.depth, 0);
		}
		return *this;
	}
	
	void Texture3D::Bind(uint32_t unit)
	{
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_2D_ARRAY, id);
	}

	void Texture3D::Unbind()
	{
		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	}

	void Texture3D::Delete()
	{
		if(id > 0)
		{
			glDeleteTextures(1, &id);
			id = 0;
		}
	}

	uint32_t Texture3D::GetWidth() const
	{
		return width;
	}

	uint32_t Texture3D::GetHeight() const
	{
		return height;
	}

	uint32_t Texture3D::GetDepth() const
	{
		return depth;
	}

	void Texture3D::CopyToTexture2D(uint32_t layerIndex, const Texture2D *texture)
	{
		if(id == 0)
			return;

		if(layerIndex >= depth)
			return;

    	glBindTexture(GL_TEXTURE_2D_ARRAY, id);
    	glBindTexture(GL_TEXTURE_2D, texture->GetId());

		glCopyImageSubData(id, GL_TEXTURE_2D_ARRAY, 0, 0, 0, layerIndex,
						texture->GetId(), GL_TEXTURE_2D, 0, 0, 0, 0,
						width, height, 1);

		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}