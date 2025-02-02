#ifndef GFX_VERTEXARRAYOBJECT_HPP
#define GFX_VERTEXARRAYOBJECT_HPP

#include "../../External/glad/glad.h"

namespace GFX
{
    class VertexArrayObject
    {
    private:
        GLuint id;
    public:
        VertexArrayObject();
        void Generate();
        void Delete();
        void Bind();
        void Unbind();
        void EnableVertexAttribArray(GLuint index);
        void VertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);
        void VertexAttribDivisor(GLuint index, GLuint divisor);
        GLuint GetId() const;
    };
}

#endif