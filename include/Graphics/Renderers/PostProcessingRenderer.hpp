#ifndef GFX_POSTPROCESSINGRENDERER_HPP
#define GFX_POSTPROCESSINGRENDERER_HPP

#include <cstdint>
#include "../Buffers/VertexArrayObject.hpp"
#include "../Buffers/VertexBufferObject.hpp"

namespace GFX
{
	class PostProcessingRenderer
	{
	private:
		VertexArrayObject vao;
		VertexBufferObject vbo;
	public:
		void Generate();
		void Delete();
		void Render(uint32_t fbo, uint32_t shaderId, uint32_t textureId);
	};
}

#endif