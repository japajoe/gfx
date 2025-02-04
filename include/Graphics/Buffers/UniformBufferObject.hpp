#ifndef GFX_UNIFORMBUFFEROBJECT_HPP
#define GFX_UNIFORMBUFFEROBJECT_HPP

#include "../../External/glad/glad.h"
#include <vector>
#include <string>

namespace GFX
{
    class UniformBufferObject
    {
    private:
        GLuint id;
    public:
        UniformBufferObject();
        UniformBufferObject(const UniformBufferObject &other);
        UniformBufferObject(UniformBufferObject &&other) noexcept;
        UniformBufferObject& operator=(const UniformBufferObject &other);
        UniformBufferObject& operator=(UniformBufferObject &&other) noexcept;
        void Generate();
        void Delete();
        void Bind();
        void Unbind();
        void BindBufferBase(GLuint index);
        void BufferData(GLsizeiptr size, const void *data, GLenum usage);
        void BufferSubData(GLintptr offset, GLsizeiptr size, const void *data);
        GLuint GetUniformBlockIndex(GLuint shaderProgram, const char *uniformBlockName);
        void UniformBlockBinding(GLuint shaderProgram, GLuint uniformBlockIndex, GLuint uniformBlockBinding);
        void BindBlockToShader(GLuint shaderProgram, GLuint bindingIndex, const std::string &blockName);
        void ObjectLabel(const std::string &label);
        GLuint GetId() const;
        
        template<typename T>
        static UniformBufferObject Create(GLuint bindingIndex, GLuint numItems)
        {
            UniformBufferObject ubo;
            ubo.Generate();
            ubo.Bind();

            std::vector<T> data;
			data.resize(numItems);
            
            ubo.BufferData(data.size() * sizeof(T), data.data(), GL_DYNAMIC_DRAW);
            ubo.BindBufferBase(bindingIndex);
            ubo.Unbind();

            return ubo;
        }
    };
}

#endif