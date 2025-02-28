#include "GL.hpp"

namespace GFX
{
	void GL::SetDepthFunc(GLenum func)
	{
		glDepthFunc(func);
	}

	void GL::SetPolygonMode(GLenum mode)
	{
		glPolygonMode(GL_FRONT_AND_BACK, mode);
	}

	void GL::DepthTest(bool enabled)
	{
		if(enabled)
			EnableDepthTest();
		else
			DisableDepthTest();
	}

	void GL::CullFace(bool enabled)
	{
		if(enabled)
			EnableCullFace();
		else
			DisableCullFace();
	}

	void GL::BlendMode(bool enabled)
	{
		if(enabled)
			EnableBlendMode();
		else
			DisableBlendMode();
	}

	void GL::DepthMask(bool enabled)
	{
		if(enabled)
			EnableDepthMask();
		else
			DisableDepthMask();
	}

	void GL::EnableDepthTest()
	{
		glEnable(GL_DEPTH_TEST);
	}

	void GL::DisableDepthTest()
	{
		glDisable(GL_DEPTH_TEST);
	}

	void GL::EnableCullFace()
	{
		glEnable(GL_CULL_FACE);
	}

	void GL::DisableCullFace()
	{
		glDisable(GL_CULL_FACE);
	}

	void GL::EnableBlendMode()
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void GL::DisableBlendMode()
	{
		glDisable(GL_BLEND);
	}

	void GL::EnableDepthMask()
	{
		glDepthMask(GL_TRUE);
	}

	void GL::DisableDepthMask()
	{
		glDepthMask(GL_FALSE);
	}
}