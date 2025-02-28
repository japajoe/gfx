#include "TextureCubeMap.hpp"
#include "../External/glad/glad.h"
#include "../External/glm/glm.hpp"
#include <stdexcept>

namespace GFX
{
    TextureCubeMap::TextureCubeMap() : Texture()
    {
        width = 0;
		height = 0;
    }

	TextureCubeMap::TextureCubeMap(uint32_t width, uint32_t height, const Color &color) : Texture()
	{
		this->width = width;
		this->height = height;

		uint32_t channels = 4;
		uint32_t size = width * height * channels;
		uint8_t *data = new uint8_t[size];

		for(uint32_t i = 0; i < size; i += channels)
		{
			uint8_t r = (uint8_t)(glm::clamp(color.r * 255.0f, 0.0f, 255.0f));
			uint8_t g = (uint8_t)(glm::clamp(color.g * 255.0f, 0.0f, 255.0f));
			uint8_t b = (uint8_t)(glm::clamp(color.b * 255.0f, 0.0f, 255.0f));
			uint8_t a = (uint8_t)(glm::clamp(color.a * 255.0f, 0.0f, 255.0f));

			data[i+0] = r;
			data[i+1] = g;
			data[i+2] = b;
			data[i+3] = a;
		}

		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_CUBE_MAP, id);

		for(int i = 0; i < 6; i++)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

		delete[] data;
	}

    TextureCubeMap::TextureCubeMap(const std::vector<Image*> &images) : Texture()
    {
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_CUBE_MAP, id);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        for(unsigned int i = 0; i < images.size(); i++)
        {
            Image *image = images[i];

            uint8_t *data = image->GetData();
            uint32_t width = image->GetWidth();
            uint32_t height = image->GetHeight();

            this->width = image->GetWidth();
            this->height = image->GetHeight();
            
            if(data)
            {
                uint32_t channels = image->GetChannels();

                switch(channels)
                {
                    case 1:
                    {
                        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, image->GetWidth(), image->GetHeight(), 0, GL_RED, GL_UNSIGNED_BYTE, data);    
                        break;
                    }
                    case 2:
                    {
                        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, image->GetWidth(), image->GetHeight(), 0, GL_RG, GL_UNSIGNED_BYTE, data);
                        break;
                    }
                    case 3:
                    {
                        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, image->GetWidth(), image->GetHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                        break;
                    }
                    case 4:
                    {
                        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, image->GetWidth(), image->GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
                        break;
                    }
                    default:
                    {
                        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
                        glDeleteTextures(1, &id);
                        std::string error = "Failed to load texture: Unsupported number of channels: " + std::to_string(image->GetChannels());
                        throw std::invalid_argument(error.c_str());
                        break;
                    }
                }
            }
            else 
            {
                glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
                glDeleteTextures(1, &id);
                throw std::invalid_argument("Failed to load texture: No valid data passed");
            }
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

    TextureCubeMap::TextureCubeMap(const TextureCubeMap &other)
    {
		this->id = other.id;
		this->width = other.width;
		this->height = other.height;
    }

    TextureCubeMap::TextureCubeMap(TextureCubeMap &&other) noexcept
    {
		this->id = std::exchange(other.id, 0);
		this->width = std::exchange(other.width, 0);
		this->height = std::exchange(other.height, 0);
    }

    TextureCubeMap& TextureCubeMap::operator=(const TextureCubeMap &other)
    {
        if(this != &other)
        {
            this->id = other.id;
            this->width = other.width;
            this->height = other.height;
        }
        return *this;
    }

    TextureCubeMap& TextureCubeMap::operator=(TextureCubeMap &&other) noexcept
    {
        if(this != &other)
        {
            this->id = std::exchange(other.id, 0);
            this->width = std::exchange(other.width, 0);
            this->height = std::exchange(other.height, 0);
        }
        return *this;
    }

    void TextureCubeMap::Bind(uint32_t unit)
    {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_CUBE_MAP, id);
    }

    void TextureCubeMap::Unbind()
    {
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

    void TextureCubeMap::Delete()
    {
        if(id > 0)
        {
            glDeleteTextures(1, &id);
        }
    }

	uint32_t TextureCubeMap::GetWidth() const
	{
		return width;
	}

	uint32_t TextureCubeMap::GetHeight() const
	{
		return height;
	}
}