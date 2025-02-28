#include "Texture2D.hpp"
#include "../External/glad/glad.h"
#include "../External/glm/glm.hpp"
#include <stdexcept>
#include <utility>
#include <string>

namespace GFX
{
	Texture2D::Texture2D() : Texture()
	{
		width = 0;
		height = 0;
	}

	Texture2D::Texture2D(uint32_t id, uint32_t width, uint32_t height) : Texture()
	{
		this->id = id;
		this->width = width;
		this->height = height;
	}

	Texture2D::Texture2D(const Image *image) : Texture()
	{
		auto data = image->GetData();

		if(data != nullptr)
		{
			width = image->GetWidth();
			height = image->GetHeight();
			uint32_t channels = image->GetChannels();

			glGenTextures(1, &id);
			glBindTexture(GL_TEXTURE_2D, id);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			switch(channels)
			{
				case 1:
				{
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);    
					break;
				}
				case 2:
				{
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RG, GL_UNSIGNED_BYTE, data);
					break;
				}
				case 3:
				{
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
					break;
				}
				case 4:
				{
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
					break;
				}
				default:
				{
					glBindTexture(GL_TEXTURE_2D, 0);
					glDeleteTextures(1, &id);
					id = 0;
					std::string error = "Failed to load texture: Unsupported number of channels: " + std::to_string(channels);
					throw std::invalid_argument(error);
				}
			}
			
			glGenerateMipmap(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}

	Texture2D::Texture2D(const uint8_t *data, size_t size, uint32_t width, uint32_t height, uint32_t channels)
	{
		if(data != nullptr)
		{
			this->width = width;
			this->height = height;

			glGenTextures(1, &id);
			glBindTexture(GL_TEXTURE_2D, id);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			switch(channels)
			{
				case 1:
				{
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);    
					break;
				}
				case 2:
				{
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RG, GL_UNSIGNED_BYTE, data);
					break;
				}
				case 3:
				{
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
					break;
				}
				case 4:
				{
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
					break;
				}
				default:
				{
					glBindTexture(GL_TEXTURE_2D, 0);
					glDeleteTextures(1, &id);
					id = 0;
					std::string error = "Failed to load texture: Unsupported number of channels: " + std::to_string(channels);
					throw std::invalid_argument(error);
				}
			}
			
			glGenerateMipmap(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}

    Texture2D::Texture2D(uint32_t width, uint32_t height, const Color &color) : Texture()
    {
        this->id = 0;
        this->width = 0;
        this->height = 0;

        if(width == 0 || height == 0)
            throw std::invalid_argument("Failed to load texture: Texture width and height must be greater than 0");

        uint32_t channels = 4;
        uint32_t size = width * height * channels;
        uint8_t *data = new uint8_t[size];

        if(data)
        {
            this->width = width;
            this->height = height;

            for(size_t i = 0; i < size; i += channels)
            {
                uint8_t r = static_cast<uint8_t>(glm::clamp(color.r * 255.0f, 0.0f, 255.0f));
                uint8_t g = static_cast<uint8_t>(glm::clamp(color.g * 255.0f, 0.0f, 255.0f));
                uint8_t b = static_cast<uint8_t>(glm::clamp(color.b * 255.0f, 0.0f, 255.0f));
                uint8_t a = static_cast<uint8_t>(glm::clamp(color.a * 255.0f, 0.0f, 255.0f));

                data[i+0] = r;
                data[i+1] = g;
                data[i+2] = b;
                data[i+3] = a;
            }

            glGenTextures(1, &id);
            glBindTexture(GL_TEXTURE_2D, id);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            
            glGenerateMipmap(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, 0);

            delete[] data;
        } 
        else 
        {
            throw std::bad_alloc();
        }
    }

	Texture2D::Texture2D(const Texture2D &other)
	{
		this->id = other.id;
		this->width = other.width;
		this->height = other.height;
	}

	Texture2D::Texture2D(Texture2D &&other) noexcept
	{
		this->id = std::exchange(other.id, 0);
		this->width = std::exchange(other.width, 0);
		this->height = std::exchange(other.height, 0);
	}

	Texture2D& Texture2D::operator=(const Texture2D &other)
	{
		if(this != &other)
		{
			this->id = other.id;
			this->width = other.width;
			this->height = other.height;
		}
		return *this;
	}

	Texture2D& Texture2D::operator=(Texture2D &&other) noexcept
	{
		if(this != &other)
		{
			this->id = std::exchange(other.id, 0);
			this->width = std::exchange(other.width, 0);
			this->height = std::exchange(other.height, 0);
		}
		return *this;
	}
	
	void Texture2D::Bind(uint32_t unit)
	{
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_2D, id);
	}

	void Texture2D::Unbind()
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Texture2D::Delete()
	{
		if(id > 0)
		{
			glDeleteTextures(1, &id);
			id = 0;
		}
	}

	uint32_t Texture2D::GetWidth() const
	{
		return width;
	}

	uint32_t Texture2D::GetHeight() const
	{
		return height;
	}
}