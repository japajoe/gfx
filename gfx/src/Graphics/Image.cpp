#include "Image.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "../External/stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../External/stb/stb_image_write.h"
#include <cstring>
#include <vector>

namespace GFX 
{
    Image::Image() 
	{
        this->data = nullptr;
        this->width = 0;
        this->height = 0;
        this->channels = 0;
        this->hasLoaded = false;
    }

    Image::Image(const std::string &filepath) 
	{
        this->data = nullptr;
        this->width = 0;
        this->height = 0;
        this->channels = 0;
        this->hasLoaded = false;
        if(LoadFromFile(filepath)) 
		{
            this->hasLoaded = true;
        }
    }

    Image::Image(const uint8_t *compressedData, size_t size) 
	{
        this->hasLoaded = false;
        this->data = nullptr;
        this->width = 0;
        this->height = 0;
        this->channels = 0;
        if(LoadFromMemory(compressedData, size)) 
		{
            this->hasLoaded = true;
        }
    }

    Image::Image(const uint8_t *uncompressedData, size_t size, uint32_t width, uint32_t height, uint32_t channels) 
	{
        this->width = width;
        this->height = height;
        this->channels = channels;
        this->data = new uint8_t[size];
        memcpy(data, uncompressedData, size);
        this->hasLoaded = true;
    }

    Image::Image(uint32_t width, uint32_t height, uint32_t channels, float r, float g, float b, float a) 
	{
        this->hasLoaded = false;
        this->data = nullptr;
        this->width = width;
        this->height = height;
        this->channels = channels;
        if(Load(width, height, channels, r, g, b, a)) 
		{
            this->hasLoaded = true;
        }
    }

    Image::Image(const Image &other) 
	{
        data = other.data;
        width = other.width;
        height = other.height;
        channels = other.channels;
        hasLoaded = other.hasLoaded;
    }

    Image::Image(Image &&other) noexcept 
	{
        data = other.data;
        width = other.width;
        height = other.height;
        channels = other.channels;
        hasLoaded = other.hasLoaded;
    }

    Image &Image::operator=(const Image &other) 
	{
        if(this != &other) 
		{
            data = other.data;
            width = other.width;
            height = other.height;
            channels = other.channels;
            hasLoaded = other.hasLoaded;
        }
        return *this;
    }

    Image &Image::operator=(Image &&other) noexcept 
	{
        if(this != &other) 
		{
            data = other.data;
            width = other.width;
            height = other.height;
            channels = other.channels;
            hasLoaded = other.hasLoaded;
        }
        return *this;
    }

    Image::~Image() 
	{
        if(data != nullptr) 
		{
            delete[] data;
            data = nullptr;
        }
    }

    uint8_t *Image::GetData() const 
	{
        return data;
    }

    uint32_t Image::GetWidth() const 
	{
        return width;
    }

    uint32_t Image::GetHeight() const 
	{
        return height;
    }

    uint32_t Image::GetChannels() const {
        return channels;
    }

    size_t Image::GetDataSize() const 
	{
        return width * height * channels;
    }

    bool Image::IsLoaded() const 
	{
        return hasLoaded;
    }

    bool Image::LoadFromFile(const std::string &filepath) 
	{
        int width, height, channels;
        unsigned char *uncompressedData = stbi_load(filepath.c_str(), &width, &height, &channels, 0);

        if (uncompressedData) 
		{
            size_t dataSize = width * height * channels * sizeof(unsigned char);

            this->data = new uint8_t[dataSize];
            this->width = width;
            this->height = height;
            this->channels = channels;
            memcpy(this->data, uncompressedData, dataSize);
            stbi_image_free(uncompressedData);
            return true;
        }

        return false;
    }

    bool Image::LoadFromMemory(const uint8_t *data, size_t size) 
	{
        int width, height, channels;
        unsigned char *uncompressedData = stbi_load_from_memory(data, size, &width, &height, &channels, 0);

        if (uncompressedData)
		{
            size_t dataSize = width * height * channels * sizeof(unsigned char);

            this->data = new uint8_t[dataSize];

            this->width = width;
            this->height = height;
            this->channels = channels;
            memcpy(this->data, uncompressedData, dataSize);
            stbi_image_free(uncompressedData);
            return true;
        }

        return false;
    }

    bool Image::Load(uint32_t width, uint32_t height, uint32_t channels, float r, float g, float b, float a) 
	{
        if(channels < 3 || channels > 4)
            return false;

        size_t size = width * height * channels;

        if(size == 0)
            return false;

        this->data = new uint8_t[size];

        auto clamp = [] (float value, float min, float max) {
            if(value < min)
                return min;
            else if(value > max)
                return max;
            return value;
        };

        if(channels == 3) 
		{
            for(size_t i = 0; i < size; i += 3) 
			{
                uint8_t R = static_cast<uint8_t>(clamp(r * 255, 0.0, 255.0));
                uint8_t G = static_cast<uint8_t>(clamp(g * 255, 0.0, 255.0));
                uint8_t B = static_cast<uint8_t>(clamp(b * 255, 0.0, 255.0));

                data[i+0] = R;
                data[i+1] = G;
                data[i+2] = B;
            }
        } 
		else 
		{
            for(size_t i = 0; i < size; i += 4) 
			{
                uint8_t R = static_cast<uint8_t>(clamp(r * 255, 0.0, 255.0));
                uint8_t G = static_cast<uint8_t>(clamp(g * 255, 0.0, 255.0));
                uint8_t B = static_cast<uint8_t>(clamp(b * 255, 0.0, 255.0));
                uint8_t A = static_cast<uint8_t>(clamp(a * 255, 0.0, 255.0));

                data[i+0] = R;
                data[i+1] = G;
                data[i+2] = B;
                data[i+3] = A;
            }
        }
        
        return true;
    }

    bool Image::SaveAsPNG(const std::string &filepath, const void *data, size_t size, size_t width, size_t height, size_t channels) 
	{
        return stbi_write_png(filepath.c_str(), width, height, channels, data, width * channels) > 0;
    }

    Image Image::CreateGradientCircle(uint32_t width, uint32_t height, const Color &color1, const Color &color2)
    {
        uint32_t channels = 4;
        std::vector<uint8_t> pixels;
        pixels.resize(width * height * channels);

        memset(pixels.data(), 0, pixels.size());

        // Calculate the center of the circle
        float centerX = width / 2.0f;
        float centerY = height / 2.0f;
        float maxRadius = std::min(centerX, centerY);

        for (uint32_t y = 0; y < height; ++y)
        {
            for (uint32_t x = 0; x < width; ++x)
            {
                // Calculate the distance from the center
                float dx = x - centerX;
                float dy = y - centerY;
                float distance = std::sqrt(dx * dx + dy * dy);

                // Normalize the distance to a value between 0 and 1
                float normalizedDistance = distance / maxRadius;
                normalizedDistance = std::min(normalizedDistance, 1.0f); // Clamp to [0, 1]

                // Interpolate between color1 and color2 based on the normalized distance
                Color currentColor;
                currentColor.r = (1.0f - normalizedDistance) * color1.r + normalizedDistance * color2.r;
                currentColor.g = (1.0f - normalizedDistance) * color1.g + normalizedDistance * color2.g;
                currentColor.b = (1.0f - normalizedDistance) * color1.b + normalizedDistance * color2.b;

                // Check if the pixel is outside the circle
                if (distance > maxRadius)
                {
                    // Set the pixel to transparent
                    currentColor.a = 0.0f; // Fully transparent
                }
                else
                {
                    // Set the alpha value based on the gradient
                    currentColor.a = (1.0f - normalizedDistance) * color1.a + normalizedDistance * color2.a;
                }

                // Set the pixel values in the buffer
                size_t index = (y * width + x) * channels;
                pixels[index] = static_cast<uint8_t>(currentColor.r * 255);
                pixels[index + 1] = static_cast<uint8_t>(currentColor.g * 255);
                pixels[index + 2] = static_cast<uint8_t>(currentColor.b * 255);
                if (channels == 4) 
                {
                    pixels[index + 3] = static_cast<uint8_t>(currentColor.a * 255);
                }
            }
        }

        return Image(pixels.data(), pixels.size(), width, height, channels);
    }
}