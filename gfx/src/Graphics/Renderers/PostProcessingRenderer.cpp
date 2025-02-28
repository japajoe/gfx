#include "PostProcessingRenderer.hpp"
#include "../../External/glad/glad.h"
#include "../../Core/GameBehaviour.hpp"

namespace GFX
{
	void PostProcessingRenderer::Generate()
	{
		float vertices[] = {
			// Positions        // Texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,

			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f
		};


		vao.Generate();
		vao.Bind();

		vbo.Generate();
		vbo.Bind();

		vbo.BufferData(sizeof(vertices), vertices, GL_STATIC_DRAW);

		vao.VertexAttribPointer(0, 3, GL_FLOAT, false, 5 * sizeof(float), (void*)0);
		vao.EnableVertexAttribArray(0);
		vao.VertexAttribPointer(1, 2, GL_FLOAT, false, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		vao.EnableVertexAttribArray(1);

		vao.Unbind();
		vbo.Unbind();
	}

	void PostProcessingRenderer::Delete()
	{
		vao.Delete();
		vbo.Delete();
	}

	void PostProcessingRenderer::Render(uint32_t fbo, uint32_t shaderId, uint32_t textureId)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glDisable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderId);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureId);

		GameBehaviour::OnBehaviourPostProcess(shaderId);

		vao.Bind();
		glDrawArrays(GL_TRIANGLES, 0, 6);
		vao.Unbind();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}