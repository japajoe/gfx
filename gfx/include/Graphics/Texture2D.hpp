#ifndef GFX_TEXTURE2D_HPP
#define GFX_TEXTURE2D_HPP

#include "Texture.hpp"
#include "Image.hpp"
#include "Color.hpp"

namespace GFX
{
	class Texture2D : public Texture
	{
	private:
		uint32_t width;
		uint32_t height;
	public:
		Texture2D();
		Texture2D(const Image *image);
		Texture2D(const uint8_t *data, size_t size, uint32_t width, uint32_t height, uint32_t channels);
		Texture2D(uint32_t id, uint32_t width, uint32_t height);
		Texture2D(uint32_t width, uint32_t height, const Color &color);
        Texture2D(const Texture2D &other);
        Texture2D(Texture2D &&other) noexcept;
        Texture2D& operator=(const Texture2D &other);
        Texture2D& operator=(Texture2D &&other) noexcept;
		void Bind(uint32_t unit) override;
		void Unbind() override;
		void Delete() override;
		uint32_t GetWidth() const;
		uint32_t GetHeight() const;
	};
}

#endif