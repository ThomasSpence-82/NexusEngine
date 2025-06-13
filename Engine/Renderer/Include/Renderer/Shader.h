#pragma once

#include <string>
#include <unordered_map>
#include "Math/Matrix4.h"
#include "Math/Vector3.h"

namespace Nexus
{
    class Shader
    {
    public:
        Shader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource);
        ~Shader();

        void Bind() const;
        void Unbind() const;

        // Uniform setters
        void SetInt(const std::string& name, int value);
        void SetFloat(const std::string& name, float value);
        void SetFloat3(const std::string& name, const Vector3& value);
        void SetMatrix4(const std::string& name, const Matrix4& value);

        const std::string& GetName() const { return m_Name; }

        // Static creation methods
        static Shader* Create(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource);

    private:
        unsigned int CompileShader(unsigned int type, const std::string& source);
        unsigned int CreateShaderProgram(const std::string& vertexSource, const std::string& fragmentSource);
        int GetUniformLocation(const std::string& name);

    private:
        unsigned int m_RendererID;
        std::string m_Name;
        std::unordered_map<std::string, int> m_UniformLocationCache;
    };
}