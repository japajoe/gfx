#ifndef GFX_GRAPHICS2D_HPP
#define GFX_GRAPHICS2D_HPP

#include "Rectangle.hpp"
#include "Vertex2D.hpp"
#include "Font.hpp"
#include <vector>
#include <cstdint>
#include <cstdlib>
#include <functional>

namespace GFX
{
    struct DrawListItem 
    {
        uint32_t shaderId;
        uint32_t textureId;
        uint32_t vertexOffset;
        uint32_t vertexCount;
        uint32_t indiceCount;
        uint32_t indiceOffset;
        bool textureIsFont;
        bool fontHasSDF;
        Rectangle clippingRect;
        void *userData;
    };

    struct DrawCommand 
    {
        Vertex2D *vertices;
        uint32_t numVertices;
        uint32_t *indices;
        uint32_t numIndices;
        uint32_t textureId;
        uint32_t shaderId;
        bool textureIsFont;
        bool fontHasSDF;
        Rectangle clippingRect;
        void *userData;

        DrawCommand()
        {
            this->vertices = nullptr;
            this->numVertices = 0;
            this->indices = nullptr;
            this->numIndices = 0; 
            this->textureId = 0; 
            this->textureIsFont = false;
            this->fontHasSDF = false;
            this->clippingRect = Rectangle(0, 0, 0, 0);
            this->userData = nullptr;
        }
    };

    struct GLStateInfo 
    {
        bool depthTestEnabled;
        bool blendEnabled;
        int blendSrcFactor;
        int blendDstFactor;
        int blendEquation;
        int depthFunc;
    };

    enum Uniform
    {
        Uniform_Projection,
        Uniform_Resolution,
        Uniform_Texture,
        Uniform_Time,
        Uniform_IsFont,
        Uniform_FontHasSDF,
        Uniform_COUNT
    };

    enum BorderOptions_
    {
        BorderOptions_Left = 1 << 0,
        BorderOptions_Right = 1 << 1,
        BorderOptions_Top = 1 << 2,
        BorderOptions_Bottom = 1 << 3,
        BorderOptions_All = BorderOptions_Left | BorderOptions_Right | BorderOptions_Top | BorderOptions_Bottom
    };

	struct TextColorInfo
	{
		size_t index;
		Color color;
	};

	typedef int BorderOptions;

	using UniformUpdateCallback = std::function<void(uint32_t shaderId, void * userData)>;

	class Graphics2D
	{
	friend class Graphics;
	private:
        static uint32_t VAO;
        static uint32_t VBO;
        static uint32_t EBO;
        static uint32_t shaderId;
        static uint32_t textureId;
        static std::vector<uint32_t> uniforms;
        static std::vector<DrawListItem> items;
        static std::vector<Vertex2D> vertices;
        static std::vector<uint32_t> indices;
        static uint32_t itemCount;
        static uint32_t vertexCount;
        static uint32_t indiceCount;
        static std::vector<Vertex2D> vertexBufferTemp; //Temporary buffer used by some 'Add' functions with dynamic size requirements
        static std::vector<uint32_t> indexBufferTemp; //Temporary buffer used by some 'Add' functions with dynamic size requirements
        static std::vector<TextColorInfo> textColorInfoTemp;
        static GLStateInfo glState;
        static uint32_t numDrawCalls;
		static void Initialize();
		static void Deinitialize();
		static void NewFrame();
		static void CheckVertexBuffer(size_t numRequiredVertices);
		static void CheckIndexBuffer(size_t numRequiredIndices);
		static void CheckItemBuffer(size_t numRequiredItems);
		static void CheckTemporaryVertexBuffer(size_t numRequiredVertices);
		static void CheckTemporaryIndexBuffer(size_t numRequiredIndices);
		template <typename T>
		static void CheckTemporaryBuffer(std::vector<T>& buffer, int numRequiredItems)
		{
			// If the current size of the buffer is less than the required number of items
			if (buffer.size() < numRequiredItems) 
			{
				// Calculate new size, starting with doubling the current size
				int newSize = buffer.size() * 2;

				// Make sure the new size is large enough to hold the required items
				while (newSize < numRequiredItems) 
				{
					newSize *= 2;
				}

				// Resize the vector to the new size
				buffer.resize(newSize);
			}
		}
		static void AddVertices(const DrawCommand *command);
		static void RotateVertices(Vertex2D *vertices, size_t numVertices, float angleDegrees);
		static void StoreState();
		static void RestoreState();
		static void CreateBuffers();
		static void CreateShader();
		static void CreateTexture();
		static void ParseColorsFromText(std::string &text, std::vector<TextColorInfo> &colors, size_t &count);
	public:
		static UniformUpdateCallback onUniformUpdate;
		static void AddRectangle(const Vector2 &position, const Vector2 &size, float rotationDegrees, const Color &color, const Rectangle clippingRect = Rectangle(0, 0, 0, 0), uint32_t shaderId = 0, void *userData = nullptr);
		static void AddRectangleRounded(const Vector2 &position, const Vector2 &size, float rotationDegrees, float radius, const Color &color, const Rectangle clippingRect = Rectangle(0, 0, 0, 0), uint32_t shaderId = 0, void *userData = nullptr);
		static void AddRectangleRoundedEx(const Vector2 &position, const Vector2 &size, float rotationDegrees, float radius, float topLeftRadius, float topRightRadius, float bottomLeftRadius, float bottomRightRadius, const Color &color, const Rectangle clippingRect = Rectangle(0, 0, 0, 0), uint32_t shaderId = 0, void *userData = nullptr);
		static void AddCircle(const Vector2 &position, float radius, int segments, float rotationDegrees, const Color &color, const Rectangle clippingRect = Rectangle(0, 0, 0, 0), uint32_t shaderId = 0, void *userData = nullptr);
		static void AddTriangle(const Vector2 &position, const Vector2 &size, float rotationDegrees, const Color &color, const Rectangle clippingRect = Rectangle(0, 0, 0, 0), uint32_t shaderId = 0, void *userData = nullptr);
		static void AddBorder(const Vector2 &position, const Vector2 &size, float thickness, BorderOptions borderOptions, const Color &color, const Rectangle clippingRect = Rectangle(0, 0, 0, 0), uint32_t shaderId = 0, void *userData = nullptr);
		static void AddLine(const Vector2 &p1, Vector2 p2, float thickness, const Color &color, const Rectangle clippingRect = Rectangle(0, 0, 0, 0), uint32_t shaderId = 0, void *userData = nullptr);
		static void AddLines(const Vector2 *segments, int count, float thickness, const Color &color, const Rectangle clippingRect = Rectangle(0, 0, 0, 0), uint32_t shaderId = 0, void *userData = nullptr);
		static void AddPlotLines(const Vector2 &position, const Vector2 &size, const float *data, int valuesCount, float thickness, const Color &color, float scaleMin = 3.402823466e+38F, float scaleMax = 3.402823466e+38F, const Rectangle clippingRect = Rectangle(0, 0, 0, 0), uint32_t shaderId = 0, void *userData = nullptr);
		static void AddImage(const Vector2 &position, const Vector2 &size, float rotationDegrees, uint32_t textureId, const Color &color, Vector2 uv0 = Vector2(0, 0), Vector2 uv1 = Vector2(1, 1), const Rectangle clippingRect = Rectangle(0, 0, 0, 0), uint32_t shaderId = 0, void *userData = nullptr);
		static void AddText(const Vector2 &position, Font *font, const std::string &text, float fontSize, const Color &color, bool richText, const Rectangle clippingRect = Rectangle(0, 0, 0, 0));
		static void AddText(const Vector2 &position, Font *font, const char *text, size_t textLength, float fontSize, const Color &color, bool richText, const Rectangle clippingRect = Rectangle(0, 0, 0, 0));
        static std::string GetVertexSource();
        static std::string GetFragmentSource();
	};
}

#endif