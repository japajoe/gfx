#include "Font.hpp"
#include "../External/glad/glad.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include <iostream>
#include <utility>

namespace GFX
{
    Font::Font()
	{
        textureId = 0;
        pixelSize = 0;
        maxHeight = 0;
        lineHeight = 0;
        codePointOfFirstChar = 0;
        renderMethod = FontRenderMethod::Normal;
    }

    Font::Font(const Font &other)
	{
        this->pixelSize = other.pixelSize;
        this->textureId = other.textureId;
        this->maxHeight = other.maxHeight;
        this->lineHeight = other.lineHeight;
        this->codePointOfFirstChar = other.codePointOfFirstChar;
        this->glyphs = other.glyphs;
        this->textureData = other.textureData;
        renderMethod = other.renderMethod;
    }

	Font::Font(Font &&other) noexcept
	{
		this->pixelSize = std::exchange(other.pixelSize, 0);
		this->textureId = std::exchange(other.textureId, 0);
		this->maxHeight = std::exchange(other.maxHeight, 0);
		this->lineHeight = std::exchange(other.lineHeight, 0);
		this->codePointOfFirstChar = std::exchange(other.codePointOfFirstChar, 0);
		this->glyphs = std::move(other.glyphs);
		this->textureData = std::move(other.textureData);
        this->renderMethod = other.renderMethod;
	}

	Font& Font::operator=(const Font &other)
	{
		if(this != &other)
		{
			this->pixelSize = other.pixelSize;
			this->textureId = other.textureId;
			this->maxHeight = other.maxHeight;
			this->lineHeight = other.lineHeight;
			this->codePointOfFirstChar = other.codePointOfFirstChar;
			this->glyphs = other.glyphs;
			this->textureData = other.textureData;
            this->renderMethod = other.renderMethod;
		}
		return *this;
	}

	Font& Font::operator=(Font &&other) noexcept
	{
		if(this != &other)
		{
			this->pixelSize = std::exchange(other.pixelSize, 0);
			this->textureId = std::exchange(other.textureId, 0);
			this->maxHeight = std::exchange(other.maxHeight, 0);
			this->lineHeight = std::exchange(other.lineHeight, 0);
			this->codePointOfFirstChar = std::exchange(other.codePointOfFirstChar, 0);
			this->glyphs = std::move(other.glyphs);
			this->textureData = std::move(other.textureData);
            this->renderMethod = other.renderMethod;
		}
		return *this;
	}

    bool Font::LoadFromFile(const std::string &filepath, uint32_t pixelSize, FontRenderMethod renderMethod) 
	{
        if(textureData.size() > 0) 
		{
            std::cerr << "Could not load font because texture is already generated\n";
            return false;
        }

        this->pixelSize = pixelSize;
        this->renderMethod = renderMethod;

        FT_Library library;
        if (FT_Init_FreeType(&library)) 
		{
            std::cerr << "Could not init FreeType library\n";
            return false;
        }

        FT_Face fontFace;
        if (FT_New_Face(library, filepath.c_str(), 0, &fontFace)) 
		{
            FT_Done_FreeType(library);
            std::cerr << "Could not load font\n";
            return false;
        }

        bool result = Load(fontFace);

        FT_Done_Face(fontFace);
        FT_Done_FreeType(library);

        return result;
    }

    bool Font::LoadFromMemory(const void *data, size_t dataSize, uint32_t pixelSize, FontRenderMethod renderMethod) 
	{
        if(textureData.size() > 0) 
		{
            std::cerr << "Could not load font because texture is already generated\n";
            return false;
        }

        this->pixelSize = pixelSize;
        this->renderMethod = renderMethod;

        FT_Library library;
        if (FT_Init_FreeType(&library)) 
		{
            std::cerr << "Could not init FreeType library\n";
            return false;
        }

        FT_Face fontFace;
        const unsigned char *pData = reinterpret_cast<const FT_Byte*>(data);
        if (FT_New_Memory_Face(library, pData, dataSize, 0, &fontFace)) 
		{
            FT_Done_FreeType(library);
            std::cerr << "Could not load font\n";
            return false;
        }

        bool result = Load(fontFace);

        FT_Done_Face(fontFace);
        FT_Done_FreeType(library);

        return result;
    }

    bool Font::Load(FT_Face fontFace) 
	{
        FT_Set_Pixel_Sizes(fontFace, 0, pixelSize);
        
        const uint32_t textureWidth = 1024;
        const uint32_t textureHeight = 1024;
        const uint32_t padding = 2;
        int row = 0;
        int col = 0;
        int64_t height = 0;

        //FT_Int32 loadFlags = FT_LOAD_RENDER | FT_LOAD_TARGET_(FT_RENDER_MODE_SDF);
        FT_Int32 loadFlags = FT_LOAD_RENDER;

        textureData.resize(textureWidth * textureHeight);

        codePointOfFirstChar = 32;
        const uint32_t charsToIncludeInFontAtlas = 95;
        const FT_ULong start = static_cast<FT_ULong>(codePointOfFirstChar);
        const FT_ULong end = start + static_cast<FT_ULong>(charsToIncludeInFontAtlas);
        glyphs.resize(charsToIncludeInFontAtlas);
        size_t index = 0;
        int64_t lineHeight = 0;
        size_t maxRowHeight = 0;

        //for(FT_ULong glyphIdx = 0; glyphIdx < numGlyphs; glyphIdx++) {
        for(FT_ULong glyphIdx = start; glyphIdx < end; glyphIdx++) 
		{
            if(FT_Load_Char(fontFace, glyphIdx, loadFlags))
                continue;

            if(FT_Render_Glyph(fontFace->glyph, renderMethod == FontRenderMethod::Normal ? FT_RENDER_MODE_NORMAL : FT_RENDER_MODE_SDF))
                continue;

            if(fontFace->glyph->bitmap.rows > maxRowHeight)
                maxRowHeight = fontFace->glyph->bitmap.rows;

            if(col + fontFace->glyph->bitmap.width + padding >= textureWidth) 
			{
                col = padding;
                row += maxRowHeight + padding;
                maxRowHeight = 0;
            }

            maxHeight = (uint32_t)std::max((int64_t)(fontFace->size->metrics.ascender - fontFace->size->metrics.descender) >> 6, height);

            for(FT_UInt y = 0; y < fontFace->glyph->bitmap.rows; y++) 
			{
                for(FT_UInt x = 0; x < fontFace->glyph->bitmap.width; x++) 
				{
                    FT_UInt indexA = (row + y) * textureWidth + col + x;
                    FT_UInt indexB = y * fontFace->glyph->bitmap.width + x;
                    textureData[indexA] = fontFace->glyph->bitmap.buffer[indexB];
                }
            }

            Glyph* glyph = &glyphs[index++];
            glyph->sizeX = fontFace->glyph->bitmap.width;
            glyph->sizeY = fontFace->glyph->bitmap.rows;
            glyph->advanceX = fontFace->glyph->advance.x >> 6;
            glyph->advanceY = fontFace->glyph->advance.y >> 6;
            glyph->bearingX = fontFace->glyph->bitmap_left;
            glyph->bearingY = fontFace->glyph->bitmap_top;
            glyph->height = fontFace->glyph->metrics.height >> 6;
            glyph->bottomBearing = (fontFace->glyph->bitmap.rows - fontFace->glyph->bitmap_top);
            glyph->leftBearing = (fontFace->glyph->bitmap.width - fontFace->glyph->bitmap_left);

            lineHeight = std::max(glyph->height, lineHeight);

            glyph->u0 = static_cast<float>(col) / textureWidth;
            glyph->v0 = static_cast<float>(row) / textureHeight;
            glyph->u1 = static_cast<float>(col + fontFace->glyph->bitmap.width) / textureWidth;
            glyph->v1 = static_cast<float>(row + fontFace->glyph->bitmap.rows) / textureHeight;

            col += fontFace->glyph->bitmap.width + padding;
        }

        this->lineHeight = static_cast<uint32_t>(lineHeight);

        return true;
    }

    bool Font::GenerateTexture() 
	{
        if(textureId > 0) {
            std::cerr << "Could not generate texture because it is already generated\n";
            return false;
        }

        const uint32_t textureWidth = 1024;
        const uint32_t textureHeight = 1024;

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, textureWidth, textureHeight, 0, GL_RED, GL_UNSIGNED_BYTE, textureData.data());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);

        //Image::saveAsPNG("test.png", textureData.data(), textureData.size(), textureWidth, textureHeight, 1);

        return textureId > 0;
    }

    void Font::Destroy() 
	{
        if(textureId > 0) 
		{
            glDeleteTextures(1, &textureId);
            textureId = 0;
        }
        if(textureData.size() > 0) 
		{
            textureData.clear();
        }
    }

    uint32_t Font::GetPixelSize() const 
	{
        return pixelSize;
    }

    uint32_t Font::GetMaxHeight() const 
	{
        return maxHeight;
    }

    uint32_t Font::GetTexture() const 
	{
        return textureId;
    }

    Glyph *Font::GetGlyph(char c) 
	{
        uint32_t index = (c - codePointOfFirstChar);
        if(index >= glyphs.size())
            return nullptr;
        return &glyphs[index];
    }

    uint32_t Font::GetCodePointOfFirstChar() const 
	{
        return codePointOfFirstChar;
    }

    void Font::CalculateBounds(const char *text, size_t size, float fontSize, float &width, float &height) 
	{
        width = 0;
        height = 0;

        int32_t maxHeight = 0; // Maximum height of any character
        int32_t currentLineWidth = 0; // Width of the current line
        int32_t currentHeight = 0;
        int32_t lineCount = 1; // Count of lines

        for(size_t i = 0; i < size; i++) 
		{
            char c = text[i];

            if (c == '\n') 
			{
                // End of a line
                if (currentLineWidth > width) 
				{
                    width = static_cast<float>(currentLineWidth);
                }
                currentHeight += GetMaxHeight();

                currentLineWidth = 0; // Reset for the next line
                lineCount++; // Increment line count
                continue;
            }

            Glyph *glyph = GetGlyph(c);

            if (!glyph)
                continue;

            // Accumulate the width using the advanceX of the glyph
            currentLineWidth += glyph->advanceX;

            // Update the maximum height based on the glyph's height
            int32_t glyphHeight = glyph->height;
            if (glyphHeight > maxHeight) 
			{
                maxHeight = glyphHeight;
            }
        }

        // Check the last line
        if (currentLineWidth > width) 
		{
            width = static_cast<float>(currentLineWidth);
        }

        if(lineCount > 1) 
		{
            height = currentHeight + maxHeight;
        } 
		else 
		{
            height = maxHeight;
        }

        width *= GetPixelScale(fontSize);
        height *= GetPixelScale(fontSize);
    }

    void Font::CalculateCharacterPosition(const char *text, size_t size, size_t characterIndex, float fontSize, float &x, float &y) 
	{
        if(size == 0)
            return;

        float startPosX = x;
        float startPosY = y;
        float characterPosX = x;
        float characterPosY = y;

        // Calculate the character position based on the character index
        for (int i = 0; i < characterIndex; ++i) 
		{
            char ch = text[i];

            // Handle line breaks
            if (ch == '\n') 
			{
                characterPosX = startPosX; // Reset X position for a new line
                characterPosY += GetMaxHeight() * GetPixelScale(fontSize);
                continue;
            }

            Glyph *glyph = GetGlyph(ch);
            if(!glyph)
                continue;

            // Update the character position based on the glyph's x advance
            characterPosX += glyph->advanceX * GetPixelScale(fontSize);
        }

        x = characterPosX;
        y = characterPosY;
    }

    float Font::CalculateYOffset(const char *text, size_t size, float fontSize)
	{
        float height = 0.0f;
        float yOffset = 0.0f;
        
        for(size_t i = 0; i < size; i++) {
            if(text[i] == '\n')
                break;

            Glyph *glyph = GetGlyph(text[i]);
            
            if(!glyph)
                continue;            

            float h = glyph->bearingY;

            if(h > height) {
                height = h;
                yOffset = (glyph->bearingY - glyph->bottomBearing) * GetPixelScale(fontSize);
            }
        }

        return yOffset;
    }

	float Font::CalculateYOffset(float fontSize) const
	{
		float height = 0.0f;
		float yOffset = 0.0f;

		char nl = '\n';
		uint32_t newLine = static_cast<uint32_t>(nl);
		uint32_t newLineIndex = (newLine - codePointOfFirstChar);
		
		for(size_t i = 0; i < glyphs.size(); i++) 
		{
			if(i == newLineIndex)
				continue;

			float h = glyphs[i].bearingY;

			if(h > height) 
			{
				height = h;
				yOffset = (glyphs[i].bearingY - glyphs[i].bottomBearing) * GetPixelScale(fontSize);
			}
		}

		return yOffset;
	}

    float Font::GetPixelScale(float fontSize) const 
	{
        return fontSize / pixelSize;
    }

    float Font::GetLineHeight() const 
	{
        return (float)lineHeight;
    }

    FontRenderMethod Font::GetRenderMethod() const
    {
        return renderMethod;
    }
}