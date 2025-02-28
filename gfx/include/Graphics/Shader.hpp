#ifndef GFX_SHADER_HPP
#define GFX_SHADER_HPP

#include <cstdint>
#include <string>
#include <unordered_map>

namespace GFX
{
    enum class ShaderType
    {
        Vertex,
        Geometry,
        Fragment,
        Program
    };

    class Shader
    {
    friend class Graphics;
    private:
        uint32_t id;
        static std::unordered_map<std::string, std::string> includesMap;
        static bool CheckShader(uint32_t shader, ShaderType type, const std::string &source);
        static std::string AddIncludes(const std::string &shaderSource);
        static void InitializeIncludes();
    public:
        Shader();
        Shader(const std::string &vertexSource, const std::string &fragmentSource);
        Shader(const std::string &vertexSource, const std::string &fragmentSource, const std::string &geometrySource);
        uint32_t GetId() const;
        void Use();
        void Delete();
        void SetMat2(const std::string &name, const float *value, bool transpose = false);
        void SetMat3(const std::string &name, const float *value, bool transpose = false);
        void SetMat4(const std::string &name, const float *value, bool transpose = false);
        void SetFloat(const std::string &name, float value);
        void SetFloat2(const std::string &name, const float *value);
        void SetFloat3(const std::string &name, const float *value);
        void SetFloat4(const std::string &name, const float *value);
        void SetInt(const std::string &name, int32_t value);
        void SetBool(const std::string &name, bool value);
        void SetMat2(int32_t location, const float *value, bool transpose = false);
        void SetMat3(int32_t location, const float *value, bool transpose = false);
        void SetMat4(int32_t location, const float *value, bool transpose = false);
        void SetFloat(int32_t location, float value);
        void SetFloat2(int32_t location, const float *value);
        void SetFloat3(int32_t location, const float *value);
        void SetFloat4(int32_t location, const float *value);
        void SetInt(int32_t location, int32_t value);
        void SetBool(int32_t location, bool value);
        static void AddIncludeFile(const std::string &name, const std::string &code);
    };
}

#endif