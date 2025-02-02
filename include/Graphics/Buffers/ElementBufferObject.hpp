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