#ifndef GFX_ELEMENTBUFFEROBJECT_HPP
#define GFX_ELEMENTBUFFEROBJECT_HPP

#include "../../External/glad/glad.h"

namespace GFX
{
    class ElementBufferObject
    {
    private:
        GLuint id;
    public:
        ElementBufferObject();
        ElementBufferObject(const ElementBufferObject &other);
        ElementBufferObject(ElementBufferObject &&other) noexcept;
        ElementBufferObject& operator=(const ElementBufferObject &other);
        ElementBufferObject& operator=(ElementBufferObject &&other) noexcept;
        void Generate();
        void Delete();
        void Bind();
        void Unbind();
        void BufferData(GLsizeiptr size, const void *data, GLenum usage);
        void BufferSubData(GLintptr offset, GLsizeiptr size, const void *data);
        GLuint GetId() const;
    };
}

#endif