#ifndef GFX_FONT_HPP
#define GFX_FONT_HPP

#include "Image.hpp"
#include <cstdint>
#include <cstdlib>
#include <string>
#include <vector>
#include <unordered_map>

typedef struct FT_FaceRec_* FT_Face;

namespace GFX 
{
    struct Glyph 
	{
        int32_t sizeX, sizeY;
        int32_t advanceX, advanceY;
        int32_t bearingX, bearingY;
        int64_t height;
        int32_t bottomBearing;
        int32_t leftBearing;
        float u0, v0;
        float u1, v1;
    };

    enum class FontRenderMethod
    {
        Normal,
        SDF
    };

    class Font 
	{
    public:
        Font();
        Font(const Font &other);
        Font(Font &&other) noexcept;
        Font& operator=(const Font &other);
        Font& operator=(Font &&other) noexcept;
        bool LoadFromFile(const std::string &filepath, uint32_t pixelSize, FontRenderMethod renderMethod);
        bool LoadFromMemory(const void *data, size_t dataSize, uint32_t pixelSize, FontRenderMethod renderMethod);
        bool GenerateTexture();
        void Destroy();
        uint32_t GetPixelSize() const;
        uint32_t GetMaxHeight() const;
        uint32_t GetTexture() const;
        Glyph *GetGlyph(char c);
        uint32_t GetCodePointOfFirstChar() const;
        void CalculateBounds(const char *text, size_t size, float fontSize, float &width, float &height);
        void CalculateCharacterPosition(const char *text, size_t size, size_t characterIndex, float fontSize, float &x, float &y);
        float CalculateYOffset(const char *text, size_t size, float fontSize);
		float CalculateYOffset(float fontSize) const;
        float GetPixelScale(float fontSize) const;
        float GetLineHeight() const;
        FontRenderMethod GetRenderMethod() const;
    private:
        uint32_t pixelSize;
        uint32_t textureId;
        uint32_t maxHeight;
        uint32_t lineHeight;
        uint32_t codePointOfFirstChar;
        std::vector<Glyph> glyphs;
        std::vector<uint8_t> textureData;
        FontRenderMethod renderMethod;
        bool Load(FT_Face fontFace);
    };
}

#endif