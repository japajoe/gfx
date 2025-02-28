#include "Shader.hpp"
#include "Shaders/CoreShaderInclude.hpp"
#include "../Core/Debug.hpp"
#include "../System/String.hpp"
#include "../External/glad/glad.h"

namespace GFX
{
    std::unordered_map<std::string, std::string> Shader::includesMap;

    static uint32_t Compile(const std::string &source, GLenum type)
    {
        uint32_t shader = glCreateShader(type);
        const char *src = source.c_str();
        glShaderSource(shader, 1, &src, NULL);
        glCompileShader(shader);
        return shader;
    }

    Shader::Shader()
    {
        id = 0;
    }

    Shader::Shader(const std::string &vertexSource, const std::string &fragmentSource)
    {
        id = 0;

        std::string sVertexSource = AddIncludes(vertexSource);
        std::string sFragmentSource = AddIncludes(fragmentSource);    

        uint32_t vertexShader = Compile(sVertexSource, GL_VERTEX_SHADER);
        
        if(!CheckShader(vertexShader, ShaderType::Vertex, sVertexSource))
        {
            glDeleteShader(vertexShader);
            return;
        }

        uint32_t fragmentShader = Compile(sFragmentSource, GL_FRAGMENT_SHADER);
        
        if(!CheckShader(fragmentShader, ShaderType::Fragment, sFragmentSource))
        {
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
            return;
        }

        id = glCreateProgram();

        glAttachShader(id, vertexShader);
        glAttachShader(id, fragmentShader);
        glLinkProgram(id);
        
        if(!CheckShader(id, ShaderType::Program, sFragmentSource))
        {
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
            glDeleteProgram(id);
            id = 0;
            return;
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    Shader::Shader(const std::string &vertexSource, const std::string &fragmentSource, const std::string &geometrySource)
    {
        id = 0;

        std::string sVertexSource = AddIncludes(vertexSource);
        std::string sGeometrySource = AddIncludes(geometrySource);
        std::string sFragmentSource = AddIncludes(fragmentSource);

        uint32_t vertexShader = Compile(sVertexSource, GL_VERTEX_SHADER);
        
        if(!CheckShader(vertexShader, ShaderType::Vertex, sVertexSource))
        {
            glDeleteShader(vertexShader);
            return;
        }

        uint32_t geometryShader = Compile(sGeometrySource, GL_GEOMETRY_SHADER);
        
        if(!CheckShader(geometryShader, ShaderType::Geometry, sGeometrySource))
        {
            glDeleteShader(vertexShader);
            glDeleteShader(geometryShader);
            return;
        }

        uint32_t fragmentShader = Compile(sFragmentSource, GL_FRAGMENT_SHADER);
        
        if(!CheckShader(fragmentShader, ShaderType::Fragment, sFragmentSource))
        {
            glDeleteShader(vertexShader);
            glDeleteShader(geometryShader);
            glDeleteShader(fragmentShader);
            return;
        }

        // Link shaders
        id = glCreateProgram();

        glAttachShader(id, vertexShader);
        glAttachShader(id, geometryShader);
        glAttachShader(id, fragmentShader);
        glLinkProgram(id);

        if(!CheckShader(id, ShaderType::Program, sFragmentSource))
        {
            glDeleteShader(vertexShader);
            glDeleteShader(geometryShader);
            glDeleteShader(fragmentShader);
            glDeleteProgram(id);
            id = 0;
            return;
        }

        glDeleteShader(vertexShader);
        glDeleteShader(geometryShader);
        glDeleteShader(fragmentShader);
    }

    uint32_t Shader::GetId() const
    {
        return id;
    }

    void Shader::Use()
    {
        glUseProgram(id);
    }

    void Shader::Delete()
    {
        if(id > 0)
        {
            glDeleteShader(id);
            id = 0;
        }
    }

    bool Shader::CheckShader(uint32_t shader, ShaderType type, const std::string &source)
    {
        int32_t success;
        GLchar infoLog[1024];
        if (type == ShaderType::Vertex || type == ShaderType::Fragment || type == ShaderType::Geometry)
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                
                std::string shaderType;

                switch(type)
                {
                    case ShaderType::Vertex:
                        shaderType = "VERTEX";
                        break;
                    case ShaderType::Geometry:
                        shaderType = "GEOMETRY";
                        break;
                    case ShaderType::Fragment:
                        shaderType = "FRAGMENT";
                        break;
                }
                
                Debug::WriteError("------------------------");
                Debug::WriteError("ERROR: SHADER_COMPILATION_ERROR of type: %s%s", shaderType.c_str(), infoLog);
                
                return false;
            }
        }
        else if (type == ShaderType::Program)
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                Debug::WriteError("------------------------");
                Debug::WriteError("ERROR: PROGRAM_LINKING_ERROR of type: PROGRAM%s", infoLog);
                return false;
            }
        }

        return true;
    }

    void Shader::SetMat2(const std::string &name, const float *value, bool transpose)
    {
        glUniformMatrix2fv(glGetUniformLocation(id, name.c_str()), 1, transpose, value);
    }

    void Shader::SetMat3(const std::string &name, const float *value, bool transpose)
    {
        glUniformMatrix3fv(glGetUniformLocation(id, name.c_str()), 1, transpose, value);
    }

    void Shader::SetMat4(const std::string &name, const float *value, bool transpose)
    {
        glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, transpose, value);
    }

    void Shader::SetFloat(const std::string &name, float value)
    {
        glUniform1fv(glGetUniformLocation(id, name.c_str()), 1, &value);
    }

    void Shader::SetFloat2(const std::string &name, const float *value)
    {
        glUniform2fv(glGetUniformLocation(id, name.c_str()), 1, value);
    }

    void Shader::SetFloat3(const std::string &name, const float *value)
    {
        glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, value);
    }

    void Shader::SetFloat4(const std::string &name, const float *value)
    {
        glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, value);
    }

    void Shader::SetInt(const std::string &name, int32_t value)
    {
        glUniform1i(glGetUniformLocation(id, name.c_str()), value);
    }

    void Shader::SetBool(const std::string &name, bool value)
    {
        glUniform1i(glGetUniformLocation(id, name.c_str()), value ? 1 : 0);
    }

    void Shader::SetMat2(int32_t location, const float *value, bool transpose)
    {
        glUniformMatrix2fv(location, 1, transpose, value);
    }

    void Shader::SetMat3(int32_t location, const float *value, bool transpose)
    {
        glUniformMatrix3fv(location, 1, transpose, value);
    }

    void Shader::SetMat4(int32_t location, const float *value, bool transpose)
    {
        glUniformMatrix4fv(location, 1, transpose, value);
    }

    void Shader::SetFloat(int32_t location, float value)
    {
        glUniform1fv(location, 1, &value);
    }

    void Shader::SetFloat2(int32_t location, const float *value)
    {
        glUniform2fv(location, 1, value);
    }

    void Shader::SetFloat3(int32_t location, const float *value)
    {
        glUniform3fv(location, 1, value);
    }

    void Shader::SetFloat4(int32_t location, const float *value)
    {
        glUniform4fv(location, 1, value);
    }

    void Shader::SetInt(int32_t location, int32_t value)
    {
        glUniform1i(location, value);
    }

    void Shader::SetBool(int32_t location, bool value)
    {
        glUniform1i(location, value ? 1 : 0);
    }

    void Shader::AddIncludeFile(const std::string &name, const std::string &code)
    {
        if(includesMap.count(name) > 0)
            return;
        includesMap[name] = code;
    }

    void Shader::InitializeIncludes()
    {
        if(includesMap.size() > 0)
            return;

        includesMap["Core"] = CoreShaderInclude::GetSource();
    }

    std::string Shader::AddIncludes(const std::string &shaderSource)
    {
        InitializeIncludes();

        if(String::Contains(shaderSource, "#include <"))
        {
            auto lines = String::Split(shaderSource, '\n');

            if(lines.size() == 0)
                return shaderSource;

            bool modified = false;

            for(size_t i = 0; i < lines.size(); i++)
            {
                if(!String::Contains(lines[i], "#include <"))
                {
                    lines[i] += "\n";
                    continue;
                }

                if(!String::Contains(lines[i], ">"))
                {
                    lines[i] += "\n";
                    continue;
                }

                String::Replace(lines[i], "<", "");
                String::Replace(lines[i], ">", "");
                String::Replace(lines[i], "#include ", "");

                if(includesMap.contains(lines[i]))
                {
                    lines[i] = includesMap[lines[i]];
                    modified = true;
                }
                else
                {
                    lines[i] += "\n";
                }
            }

            if(modified)
            {
                std::string newSource;

                for(int i = 0; i < lines.size(); i++)
                {
                    newSource += lines[i];
                }

                return newSource;
            }
        }

        return shaderSource;
    }
}