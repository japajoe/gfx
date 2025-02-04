#ifndef GFX_CUBEMAPTEXTURE_HPP
#define GFX_CUBEMAPTEXTURE_HPP

#include "Texture.hpp"
#include "Image.hpp"
#include <vector>
#include "Color.hpp"

namespace GFX
{
    class TextureCubeMap : public Texture
    {
	private:
		uint32_t width;
		uint32_t height;
    public:
        TextureCubeMap();
		TextureCubeMap(uint32_t width, uint32_t height, const Color &color);
        //Order: right, left, bottom, top, front, back
        TextureCubeMap(const std::vector<Image*> &images);
        TextureCubeMap(const TextureCubeMap &other);
        TextureCubeMap(TextureCubeMap &&other) noexcept;
        TextureCubeMap& operator=(const TextureCubeMap &other);
        TextureCubeMap& operator=(TextureCubeMap &&other) noexcept;
        void Bind(uint32_t unit) override;
        void Unbind() override;
        void Delete() override;
		uint32_t GetWidth() const;
		uint32_t GetHeight() const;
    };
}

#endif