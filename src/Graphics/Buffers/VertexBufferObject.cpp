#include "VertexBufferObject.hpp"

namespace GFX
{
    VertexBufferObject::VertexBufferObject()
    {
        id = 0;
    }

    void VertexBufferObject::Generate()
    {
        glGenBuffers(1, &id);
    }

    void VertexBufferObject::Delete()
    {
        if(id > 0)
        {
            glDeleteBuffers(1, &id);
            id = 0;
        }
    }
    
    void VertexBufferObject::Bind()
    {
        glBindBuffer(GL_ARRAY_BUFFER, id);
    }

    void VertexBufferObject::Unbind()
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void VertexBufferObject::BufferData(GLsizeiptr size, const void *data, GLenum usage)
    {
        glBufferData(GL_ARRAY_BUFFER, size, data, usage);
    }

    void VertexBufferObject::BufferSubData(GLintptr offset, GLsizeiptr size, const void *data)
    {
        glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
    }

    GLuint VertexBufferObject::GetId() const
    {
        return id;
    }
}