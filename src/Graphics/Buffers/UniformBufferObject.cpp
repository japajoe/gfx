#include "UniformBufferObject.hpp"

namespace GFX
{
    UniformBufferObject::UniformBufferObject()
    {
        this->id = 0;
    }

    void UniformBufferObject::Generate()
    {
        glGenBuffers(1, &id);
    }

    void UniformBufferObject::Delete()
    {
        if(id > 0)
        {
            glDeleteBuffers(1, &id);
            id = 0;
        }
    }

    void UniformBufferObject::Bind()
    {
        glBindBuffer(GL_UNIFORM_BUFFER, id);
    }

    void UniformBufferObject::Unbind()
    {
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    void UniformBufferObject::BindBufferBase(GLuint index)
    {
        glBindBufferBase(GL_UNIFORM_BUFFER, index, id);
    }

    void UniformBufferObject::BufferData(GLsizeiptr size, const void *data, GLenum usage)
    {
        glBufferData(GL_UNIFORM_BUFFER, size, data, usage);
    }

    void UniformBufferObject::BufferSubData(GLintptr offset, GLsizeiptr size, const void *data)
    {
        glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
    }

	GLuint UniformBufferObject::GetUniformBlockIndex(GLuint shaderProgram, const char *uniformBlockName)
	{
		return glGetUniformBlockIndex(shaderProgram, uniformBlockName);
	}

	void UniformBufferObject::UniformBlockBinding(GLuint shaderProgram, GLuint uniformBlockIndex, GLuint uniformBlockBinding)
	{
		glUniformBlockBinding(shaderProgram, uniformBlockIndex, uniformBlockBinding);
	}

	void UniformBufferObject::BindBlockToShader(GLuint shaderProgram, GLuint bindingIndex, const char *blockName)
	{
		GLuint blockIndex = glGetUniformBlockIndex(shaderProgram, blockName);
		glUniformBlockBinding(shaderProgram, blockIndex, bindingIndex);
	}
}