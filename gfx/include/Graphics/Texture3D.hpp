#ifndef GFX_TEXTURE3D_HPP
#define GFX_TEXTURE3D_HPP

#include "Texture.hpp"

namespace GFX
{
	class Texture2D;

	class Texture3D : public Texture
	{
	private:
		uint32_t width;
		uint32_t height;
        uint32_t depth;
	public:
		Texture3D();
		Texture3D(uint32_t width, uint32_t height, uint32_t depth);
        Texture3D(const Texture3D &other);
        Texture3D(Texture3D &&other) noexcept;
        Texture3D& operator=(const Texture3D &other);
        Texture3D& operator=(Texture3D &&other) noexcept;
		void Bind(uint32_t unit) override;
		void Unbind() override;
		void Delete() override;
		uint32_t GetWidth() const;
		uint32_t GetHeight() const;
        uint32_t GetDepth() const;
		void CopyToTexture2D(uint32_t layerIndex, const Texture2D *texture);
	};
}

#endif