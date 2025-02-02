#ifndef GFX_UNIFORMBUFFEROBJECT_HPP
#define GFX_UNIFORMBUFFEROBJECT_HPP

#include "../../External/glad/glad.h"

namespace GFX
{
    class UniformBufferObject
    {
    private:
        GLuint id;
    public:
        UniformBufferObject();
        void Generate();
        void Delete();
        void Bind();
        void Unbind();
        void BindBufferBase(GLuint index);
        void BufferData(GLsizeiptr size, const void *data, GLenum usage);
        void BufferSubData(GLintptr offset, GLsizeiptr size, const void *data);
        GLuint GetUniformBlockIndex(GLuint shaderProgram, const char *uniformBlockName);
        void UniformBlockBinding(GLuint shaderProgram, GLuint uniformBlockIndex, GLuint uniformBlockBinding);
        void BindBlockToShader(GLuint shaderProgram, GLuint bindingIndex, const char *blockName);
        GLuint GetId() const;
    };
}

#endif