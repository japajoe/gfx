#ifndef GFX_FRAMEBUFFEROBJECT_HPP
#define GFX_FRAMEBUFFEROBJECT_HPP

#include "../../External/glad/glad.h"
#include <cstdint>

namespace GFX
{
	class FrameBufferObject
	{
	private:
		GLuint id;
        GLuint textureAttachmentId;
        GLuint depthAttachmentId;
        uint32_t width;
        uint32_t height;
	public:
    	FrameBufferObject();
    	FrameBufferObject(uint32_t width, uint32_t height);
        FrameBufferObject(const FrameBufferObject &other);
        FrameBufferObject(FrameBufferObject &&other) noexcept;
        FrameBufferObject& operator=(const FrameBufferObject &other);
        FrameBufferObject& operator=(FrameBufferObject &&other) noexcept;
    	void Generate();
    	void Delete();
    	void Bind();
    	void Unbind();
    	void Resize(uint32_t width, uint32_t height);
		void Resolve();
    	GLuint GetId() const;
    	GLuint GetTextureId() const;
		GLuint GetDepthId() const;
		uint32_t GetWidth() const;
		uint32_t GetHeight() const;
	};
}

#endif