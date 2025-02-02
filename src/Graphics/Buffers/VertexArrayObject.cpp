#include "VertexArrayObject.hpp"

namespace GFX
{
    VertexArrayObject::VertexArrayObject()
    {
        id = 0;
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