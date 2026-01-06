#include "FrameBufferObject.hpp"
#include "../../Core/Debug.hpp"
#include <utility>

namespace GFX
{
	FrameBufferObject::FrameBufferObject()
	{
		this->id = 0;
		this->textureAttachmentId = 0;
		this->depthAttachmentId = 0;
		this->width = 0;
		this->height = 0;
		this->multiSample = false;
		this->hdr = false;
	}

	FrameBufferObject::FrameBufferObject(uint32_t width, uint32_t height, bool multiSample, bool hdr)
	{
		this->id = 0;
		this->textureAttachmentId = 0;
		this->depthAttachmentId = 0;
		this->width = width;
		this->height = height;
		this->multiSample = multiSample;
		this->hdr = hdr;
	}

	FrameBufferObject::FrameBufferObject(const FrameBufferObject &other)
	{
		id = other.id;
		textureAttachmentId = other.textureAttachmentId;
		depthAttachmentId = other.depthAttachmentId;
		width = other.width;
		height = other.height;
		multiSample = other.multiSample;
		hdr = other.hdr;
	}

	FrameBufferObject::FrameBufferObject(FrameBufferObject &&other) noexcept
	{
		id = std::exchange(other.id, 0);
		textureAttachmentId = std::exchange(other.textureAttachmentId, 0);
		depthAttachmentId = std::exchange(other.depthAttachmentId, 0);
		width = std::exchange(other.width, 0);
		height = std::exchange(other.height, 0);
		multiSample = other.multiSample;
		hdr = other.hdr;
	}

	FrameBufferObject& FrameBufferObject::operator=(const FrameBufferObject &other)
	{
		if(this != &other)
		{
			id = other.id;
			textureAttachmentId = other.textureAttachmentId;
			depthAttachmentId = other.depthAttachmentId;
			width = other.width;
			height = other.height;
			multiSample = other.multiSample;
			hdr = other.hdr;
		}
		return *this;
	}

	FrameBufferObject& FrameBufferObject::operator=(FrameBufferObject &&other) noexcept
	{
		if(this != &other)
		{
			id = std::exchange(other.id, 0);
			textureAttachmentId = std::exchange(other.textureAttachmentId, 0);
			depthAttachmentId = std::exchange(other.depthAttachmentId, 0);
			width = std::exchange(other.width, 0);
			height = std::exchange(other.height, 0);
			multiSample = other.multiSample;
			hdr = other.hdr;
		}
		return *this;
	}

	void FrameBufferObject::Generate()
	{
		if(width == 0 || height == 0)
			return;

		Resize(width, height);
	}

	void FrameBufferObject::Delete()
	{
		if(id > 0)
		{
			glDeleteFramebuffers(1, &id);
			id = 0;
		}
		if(textureAttachmentId > 0)
		{
			glDeleteTextures(1, &textureAttachmentId);
			textureAttachmentId = 0;
		}
		if(depthAttachmentId > 0)
		{
			glDeleteRenderbuffers(1, &depthAttachmentId);
			depthAttachmentId = 0;
		}
	}

	void FrameBufferObject::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, id);	
	}

	void FrameBufferObject::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void FrameBufferObject::Resize(uint32_t width, uint32_t height)
	{
		Delete();

		this->width = width;
		this->height = height;

		glGenFramebuffers(1, &id);
		glBindFramebuffer(GL_FRAMEBUFFER, id);

		// Create color attachment
		if (multiSample)
		{
			// Create a multisampled texture
			glGenTextures(1, &textureAttachmentId);
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureAttachmentId);
			if (hdr)
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA16F, width, height, GL_TRUE); // Use GL_TRUE for fixed sample locations
			else
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA, width, height, GL_TRUE);
		}
		else
		{
			// Create a regular texture
			glGenTextures(1, &textureAttachmentId);
			glBindTexture(GL_TEXTURE_2D, textureAttachmentId);
			if (hdr)
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
			else
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		}

		// Set texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// Attach the texture to the framebuffer
		if (multiSample)
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, textureAttachmentId, 0);
		}
		else
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureAttachmentId, 0);
		}

		// Create depth/stencil attachment
		glGenRenderbuffers(1, &depthAttachmentId);
		glBindRenderbuffer(GL_RENDERBUFFER, depthAttachmentId);
		if (multiSample)
			glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, width, height);
		else
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
		
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthAttachmentId);

		// Check framebuffer status
		uint32_t status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			printf("Failed to initialize frame buffer, status: %u\n", status); // Use %u for unsigned int
			// Handle error (e.g., cleanup)
		}

		glViewport(0, 0, width, height);

		// Unbind framebuffer and other resources
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

	void FrameBufferObject::Blit(const FrameBufferObject &fbo)
	{
		// Resolve the multisampled framebuffer into another framebuffer (with non-multisampled texture)
		glBindFramebuffer(GL_READ_FRAMEBUFFER, id);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo.GetId());
		glBlitFramebuffer(0, 0, width, height, 0, 0, fbo.GetWidth(), fbo.GetHeight(), GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		Unbind();
	}

	void FrameBufferObject::Blit(uint32_t fbo, uint32_t width, uint32_t height)
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, id);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
		glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		Unbind();
	}

	GLuint FrameBufferObject::GetId() const
	{
		return id;
	}

	GLuint FrameBufferObject::GetTextureId() const
	{
		return textureAttachmentId;
	}

	GLuint FrameBufferObject::GetDepthId() const
	{
		return depthAttachmentId;
	}

	uint32_t FrameBufferObject::GetWidth() const
	{
		return width;
	}

	uint32_t FrameBufferObject::GetHeight() const
	{
		return height;
	}

    // FrameBufferObject::FrameBufferObject()
    // {
    //     this->id = 0;
    //     this->textureAttachmentId = 0;
    //     this->depthAttachmentId = 0;
    //     this->width = 0;
    //     this->height = 0;
    // }

    // FrameBufferObject::FrameBufferObject(uint32_t width, uint32_t height)
    // {
    //     this->id = 0;
    //     this->textureAttachmentId = 0;
    //     this->depthAttachmentId = 0;
    //     this->width = width;
    //     this->height = height;
    // }

	// FrameBufferObject::FrameBufferObject(const FrameBufferObject &other)
	// {
    //     id = other.id;
    //     textureAttachmentId = other.textureAttachmentId;
    //     depthAttachmentId = other.depthAttachmentId;
    //     width = other.width;
    //     height = other.height;
	// }

	// FrameBufferObject::FrameBufferObject(FrameBufferObject &&other) noexcept
	// {
    //     id = std::exchange(other.id, 0);
    //     textureAttachmentId = std::exchange(other.textureAttachmentId, 0);
    //     depthAttachmentId = std::exchange(other.depthAttachmentId, 0);
    //     width = std::exchange(other.width, 0);
    //     height = std::exchange(other.height, 0);
	// }
	
	// FrameBufferObject& FrameBufferObject::operator=(const FrameBufferObject &other)
	// {
	// 	if(this != &other)
	// 	{
	// 		id = other.id;
	// 		textureAttachmentId = other.textureAttachmentId;
	// 		depthAttachmentId = other.depthAttachmentId;
	// 		width = other.width;
	// 		height = other.height;
	// 	}
	// 	return *this;
	// }

	// FrameBufferObject& FrameBufferObject::operator=(FrameBufferObject &&other) noexcept
	// {
	// 	if(this != &other)
	// 	{
	// 		id = std::exchange(other.id, 0);
	// 		textureAttachmentId = std::exchange(other.textureAttachmentId, 0);
	// 		depthAttachmentId = std::exchange(other.depthAttachmentId, 0);
	// 		width = std::exchange(other.width, 0);
	// 		height = std::exchange(other.height, 0);
	// 	}
	// 	return *this;
	// }

    // void FrameBufferObject::Generate()
    // {
    //     if(width == 0 || height == 0)
    //         return;

    //     glGenFramebuffers(1, &id);
    //     glBindFramebuffer(GL_FRAMEBUFFER, id);

    //     glGenTextures(1, &textureAttachmentId);
    //     glBindTexture(GL_TEXTURE_2D, textureAttachmentId);
    //     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //     glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureAttachmentId, 0);

    //     glGenRenderbuffers(1, &depthAttachmentId);
    //     glBindRenderbuffer(GL_RENDERBUFFER, depthAttachmentId);
    //     glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    //     glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthAttachmentId);

    //     uint32_t status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        
    //     if(status != GL_FRAMEBUFFER_COMPLETE)
    //     {
    //         Debug::WriteError("Failed to initialize frame buffer, status: %llu", status);
    //     }

    //     glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //     glBindTexture(GL_TEXTURE_2D, 0);
    //     glBindRenderbuffer(GL_RENDERBUFFER, 0);
    // }

    // void FrameBufferObject::Delete()
    // {
    //     if(id > 0)
    //     {
    //         glDeleteFramebuffers(1, &id);
    //         id = 0;
    //         textureAttachmentId = 0;
    //         depthAttachmentId = 0;
    //     }
    // }

    // void FrameBufferObject::Bind()
    // {
    //     glBindFramebuffer(GL_FRAMEBUFFER, id);
    // }

    // void FrameBufferObject::Unbind()
    // {
    //     glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // }

    // void FrameBufferObject::Resize(uint32_t width, uint32_t height)
    // {
    //     Delete();

    //     glGenFramebuffers(1, &id);
    //     glBindFramebuffer(GL_FRAMEBUFFER, id);

    //     glGenTextures(1, &textureAttachmentId);
    //     glBindTexture(GL_TEXTURE_2D, textureAttachmentId);
    //     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //     glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureAttachmentId, 0);

    //     glGenRenderbuffers(1, &depthAttachmentId);
    //     glBindRenderbuffer(GL_RENDERBUFFER, depthAttachmentId);
    //     glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    //     glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthAttachmentId);

    //     uint32_t status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        
    //     if(status != GL_FRAMEBUFFER_COMPLETE)
    //     {
    //         Debug::WriteError("Failed to initialize frame buffer, status: %llu", status);
    //     }

    //     glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //     glBindTexture(GL_TEXTURE_2D, 0);
    //     glBindRenderbuffer(GL_RENDERBUFFER, 0);
    // }

    // // void FrameBufferObject::Resize(uint32_t width, uint32_t height)
    // // {
    // //     glBindTexture(GL_TEXTURE_2D, textureAttachmentId);
    // //     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    // //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    // //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        
    // //     glBindFramebuffer(GL_FRAMEBUFFER, id);
    // //     glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureAttachmentId, 0); 

    // //     glBindRenderbuffer(GL_RENDERBUFFER, depthAttachmentId);
    // //     glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    // //     glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthAttachmentId);

    // //     glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // //     glBindTexture(GL_TEXTURE_2D, 0);
    // //     glBindRenderbuffer(GL_RENDERBUFFER, 0);
    // // }

    // void FrameBufferObject::Resolve()
    // {
    //     // Resolve the multisampled framebuffer to the default framebuffer or a regular non-multisampled texture
    //     glBindFramebuffer(GL_READ_FRAMEBUFFER, id);
    //     glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);  // Default framebuffer (screen)
    //     glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    // }

    // GLuint FrameBufferObject::GetId() const
    // {
    //     return id;
    // }

    // GLuint FrameBufferObject::GetTextureId() const
    // {
    //     return textureAttachmentId;
    // }

	// GLuint FrameBufferObject::GetDepthId() const
	// {
	// 	return depthAttachmentId;
	// }

	// uint32_t FrameBufferObject::GetWidth() const
	// {
	// 	return width;
	// }

	// uint32_t FrameBufferObject::GetHeight() const
	// {
	// 	return height;
	// }
}