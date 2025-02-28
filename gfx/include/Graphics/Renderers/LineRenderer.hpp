#ifndef GFX_LINERENDERER_HPP
#define GFX_LINERENDERER_HPP

#include <vector>
#include "../../External/glad/glad.h"
#include "../../System/Numerics/Vector3.hpp"
#include "../Color.hpp"
#include <cstdint>

namespace GFX
{
    class Shader;

    struct LineVertex
    {
        Vector3 position;
        Color color;
        LineVertex();
        LineVertex(const Vector3 &position, const Color &color);
    };

    class LineRenderer
    {
	friend class Graphics;
    private:
        static std::vector<LineVertex> lines;
        static size_t numLines;
        static size_t pointIndex;
        static size_t maxLines;
        static uint32_t VAO;
        static uint32_t VBO;
        static Shader *shader;
        static float lineWidth;
        static void AddToDrawList(const Vector3 &p1, const Vector3 &p2, const Color &color);
        static void Clear();
        static void Initialize();
        static void Deinitialize();
		static void NewFrame();
    public:
        static void DrawLine(const Vector3 &p1, const Vector3 &p2);
        static void DrawLine(const Vector3 &p1, const Vector3 &p2, const Color &color);
        static void DrawWireArc(const Vector3 &center, const Vector3 &normal, const Vector3 &from, float angle, float radius, const Color &color);
        static void DrawWireDisc(const Vector3 &center, const Vector3 &normal, float radius, const Color &color);
        static void DrawSphere(const Vector3 &center, float radius);
        static void SetLineWidth(float width);
        static float GetLineWidth();
    };
}

#endif