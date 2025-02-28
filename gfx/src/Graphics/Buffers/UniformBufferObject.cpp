#include "UniformBufferObject.hpp"
#include <utility>

namespace GFX
{
    UniformBufferObject::UniformBufferObject()
    {
        this->id = 0;
    }

    UniformBufferObject::UniformBufferObject(const UniformBufferObject &other)
    {
        id = other.id;
    }

    UniformBufferObject::UniformBufferObject(UniformBufferObject &&other) noexcept
    {
        id = std::exchange(other.id, 0);
    }

    UniformBufferObject& UniformBufferObject::operator=(const UniformBufferObject &other)
    {
        if(this != &other)
        {
            id = other.id;
        }
        return *this;
    }

    UniformBufferObject& UniformBufferObject::operator=(UniformBufferObject &&other) noexcept
    {
        if(this != &other)
        {
            id = std::exchange(other.id, 0);
        }
        return *this;
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

	void UniformBufferObject::BindBlockToShader(GLuint shaderProgram, GLuint bindingIndex, const std::string &blockName)
	{
		GLuint blockIndex = glGetUniformBlockIndex(shaderProgram, blockName.c_str());
		glUniformBlockBinding(shaderProgram, blockIndex, bindingIndex);
	}

    void UniformBufferObject::ObjectLabel(const std::string &label)
    {
        glObjectLabel(GL_BUFFER, id, -1, label.c_str());
    }

    GLuint UniformBufferObject::GetId() const
    {
        return id;
    }
}