#ifndef GFX_VERTEXBUFFEROBJECT_HPP
#define GFX_VERTEXBUFFEROBJECT_HPP

#include "../../External/glad/glad.h"

namespace GFX
{
    class VertexBufferObject
    {
    private:
        GLuint id;
    public:
        VertexBufferObject();
        VertexBufferObject(const VertexBufferObject &other);
        VertexBufferObject(VertexBufferObject &&other) noexcept;
        VertexBufferObject& operator=(const VertexBufferObject &other);
        VertexBufferObject& operator=(VertexBufferObject &&other) noexcept;
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