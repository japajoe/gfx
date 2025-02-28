#include "VertexArrayObject.hpp"
#include <utility>

namespace GFX
{
    VertexArrayObject::VertexArrayObject()
    {
        id = 0;
    }

    VertexArrayObject::VertexArrayObject(const VertexArrayObject &other)
    {
        id = other.id;
    }

    VertexArrayObject::VertexArrayObject(VertexArrayObject &&other) noexcept
    {
        id = std::exchange(other.id, 0);
    }

    VertexArrayObject& VertexArrayObject::operator=(const VertexArrayObject &other)
    {
        if(this != &other)
        {
            id = other.id;
        }
        return *this;
    }

    VertexArrayObject& VertexArrayObject::operator=(VertexArrayObject &&other) noexcept
    {
        if(this != &other)
        {
            id = std::exchange(other.id, 0);
        }
        return *this;
    }

    void VertexArrayObject::Generate()
    {
        glGenVertexArrays(1, &id);
    }

    void VertexArrayObject::Delete()
    {
        if(id > 0)
        {
            glDeleteVertexArrays(1, &id);
            id = 0;
        }
    }
    
    void VertexArrayObject::Bind()
    {
        glBindVertexArray(id);
    }

    void VertexArrayObject::Unbind()
    {
        glBindVertexArray(0);
    }

    void VertexArrayObject::EnableVertexAttribArray(GLuint index)
    {
        glEnableVertexAttribArray(index);
    }

    void VertexArrayObject::VertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer)
    {
        glVertexAttribPointer(index, size, type, normalized, stride, pointer);
    }

    void VertexArrayObject::VertexAttribDivisor(GLuint index, GLuint divisor)
    {
        glVertexAttribDivisor(index, divisor);
    }

    GLuint VertexArrayObject::GetId() const
    {
        return id;
    }
}