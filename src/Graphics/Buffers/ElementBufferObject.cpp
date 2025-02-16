#include "ElementBufferObject.hpp"
#include <utility>

namespace GFX
{
    ElementBufferObject::ElementBufferObject()
    {
        id = 0;
    }

    ElementBufferObject::ElementBufferObject(const ElementBufferObject &other)
    {
        id = other.id;
    }

    ElementBufferObject::ElementBufferObject(ElementBufferObject &&other) noexcept
    {
        id = std::exchange(other.id, 0);
    }

    ElementBufferObject& ElementBufferObject::operator=(const ElementBufferObject &other)
    {
        if(this != &other)
        {
            id = other.id;
        }
        return *this;
    }

    ElementBufferObject& ElementBufferObject::operator=(ElementBufferObject &&other) noexcept
    {
        if(this != &other)
        {
            id = std::exchange(other.id, 0);
        }
        return *this;
    }

    void ElementBufferObject::Generate()
    {
        glGenBuffers(1, &id);
    }

    void ElementBufferObject::Delete()
    {
        if(id > 0)
        {
            glDeleteBuffers(1, &id);
            id = 0;
        }
    }
    
    void ElementBufferObject::Bind()
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
    }

    void ElementBufferObject::Unbind()
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void ElementBufferObject::BufferData(GLsizeiptr size, const void *data, GLenum usage)
    {
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, usage);
    }

    void ElementBufferObject::BufferSubData(GLintptr offset, GLsizeiptr size, const void *data)
    {
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, size, data);
    }

    uint32_t ElementBufferObject::GetId() const
    {
        return id;
    }
}