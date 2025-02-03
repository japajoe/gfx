#ifndef GFX_GL_HPP
#define GFX_GL_HPP

#include "../External/glad/glad.h"

namespace GFX
{
    class GL
    {
	private:
        static void EnableDepthTest();
        static void DisableDepthTest();
        static void EnableCullFace();
        static void DisableCullFace();
        static void EnableBlendMode();
        static void DisableBlendMode();
        static void EnableDepthMask();
        static void DisableDepthMask();
	public:
        static void SetDepthFunc(GLenum func);
        static void SetPolygonMode(GLenum mode);
        static void DepthTest(bool enabled);
        static void CullFace(bool enabled);
        static void BlendMode(bool enabled);
        static void DepthMask(bool enabled);
    };
}

#endif