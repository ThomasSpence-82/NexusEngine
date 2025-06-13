#include "Renderer/Shader.h"
#include "Core/Logger.h"
#include <windows.h>
#include <GL/gl.h>
#include <vector>

// OpenGL constants we need
#define GL_VERTEX_SHADER                  0x8B31
#define GL_FRAGMENT_SHADER                0x8B30
#define GL_COMPILE_STATUS                 0x8B81
#define GL_LINK_STATUS                    0x8B82
#define GL_INFO_LOG_LENGTH                0x8B84

// OpenGL function pointers
typedef unsigned int (APIENTRY *PFNGLCREATESHADERPROC)(unsigned int type);
typedef void (APIENTRY *PFNGLSHADERSOURCEPROC)(unsigned int shader, int count, const char* const* string, const int* length);
typedef void (APIENTRY *PFNGLCOMPILESHADERPROC)(unsigned int shader);
typedef void (APIENTRY *PFNGLGETSHADERIVPROC)(unsigned int shader, unsigned int pname, int* params);
typedef void (APIENTRY *PFNGLGETSHADERINFOLOGPROC)(unsigned int shader, int bufSize, int* length, char* infoLog);
typedef unsigned int (APIENTRY *PFNGLCREATEPROGRAMPROC)(void);
typedef void (APIENTRY *PFNGLATTACHSHADERPROC)(unsigned int program, unsigned int shader);
typedef void (APIENTRY *PFNGLLINKPROGRAMPROC)(unsigned int program);
typedef void (APIENTRY *PFNGLGETPROGRAMIVPROC)(unsigned int program, unsigned int pname, int* params);
typedef void (APIENTRY *PFNGLGETPROGRAMINFOLOGPROC)(unsigned int program, int bufSize, int* length, char* infoLog);
typedef void (APIENTRY *PFNGLUSEPROGRAMPROC)(unsigned int program);
typedef void (APIENTRY *PFNGLDELETESHADERPROC)(unsigned int shader);
typedef void (APIENTRY *PFNGLDELETEPROGRAMPROC)(unsigned int program);
typedef int (APIENTRY *PFNGLGETUNIFORMLOCATIONPROC)(unsigned int program, const char* name);
typedef void (APIENTRY *PFNGLUNIFORM1IPROC)(int location, int v0);
typedef void (APIENTRY *PFNGLUNIFORM1FPROC)(int location, float v0);
typedef void (APIENTRY *PFNGLUNIFORM3FPROC)(int location, float v0, float v1, float v2);
typedef void (APIENTRY *PFNGLUNIFORMMATRIX4FVPROC)(int location, int count, unsigned char transpose, const float* value);

namespace Nexus
{
    // Load OpenGL functions
    static PFNGLCREATESHADERPROC glCreateShader = nullptr;
    static PFNGLSHADERSOURCEPROC glShaderSource = nullptr;
    static PFNGLCOMPILESHADERPROC glCompileShader = nullptr;
    static PFNGLGETSHADERIVPROC glGetShaderiv = nullptr;
    static PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog = nullptr;
    static PFNGLCREATEPROGRAMPROC glCreateProgram = nullptr;
    static PFNGLATTACHSHADERPROC glAttachShader = nullptr;
    static PFNGLLINKPROGRAMPROC glLinkProgram = nullptr;
    static PFNGLGETPROGRAMIVPROC glGetProgramiv = nullptr;
    static PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog = nullptr;
    static PFNGLUSEPROGRAMPROC glUseProgram = nullptr;
    static PFNGLDELETESHADERPROC glDeleteShader = nullptr;
    static PFNGLDELETEPROGRAMPROC glDeleteProgram = nullptr;
    static PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation = nullptr;
    static PFNGLUNIFORM1IPROC glUniform1i = nullptr;
    static PFNGLUNIFORM1FPROC glUniform1f = nullptr;
    static PFNGLUNIFORM3FPROC glUniform3f = nullptr;
    static PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv = nullptr;

    static bool s_OpenGLFunctionsLoaded = false;

    static void LoadOpenGLFunctions()
    {
        if (s_OpenGLFunctionsLoaded) return;

        glCreateShader = (PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader");
        glShaderSource = (PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource");
        glCompileShader = (PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader");
        glGetShaderiv = (PFNGLGETSHADERIVPROC)wglGetProcAddress("glGetShaderiv");
        glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress("glGetShaderInfoLog");
        glCreateProgram = (PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram");
        glAttachShader = (PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader");
        glLinkProgram = (PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram");
        glGetProgramiv = (PFNGLGETPROGRAMIVPROC)wglGetProcAddress("glGetProgramiv");
        glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)wglGetProcAddress("glGetProgramInfoLog");
        glUseProgram = (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");
        glDeleteShader = (PFNGLDELETESHADERPROC)wglGetProcAddress("glDeleteShader");
        glDeleteProgram = (PFNGLDELETEPROGRAMPROC)wglGetProcAddress("glDeleteProgram");
        glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation");
        glUniform1i = (PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i");
        glUniform1f = (PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f");
        glUniform3f = (PFNGLUNIFORM3FPROC)wglGetProcAddress("glUniform3f");
        glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)wglGetProcAddress("glUniformMatrix4fv");

        s_OpenGLFunctionsLoaded = true;
        NEXUS_CORE_INFO("OpenGL shader functions loaded");
    }

    Shader::Shader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource)
        : m_Name(name), m_RendererID(0)
    {
        LoadOpenGLFunctions();
        m_RendererID = CreateShaderProgram(vertexSource, fragmentSource);
    }

    Shader::~Shader()
    {
        if (glDeleteProgram && m_RendererID)
            glDeleteProgram(m_RendererID);
    }

    void Shader::Bind() const
    {
        if (glUseProgram)
            glUseProgram(m_RendererID);
    }

    void Shader::Unbind() const
    {
        if (glUseProgram)
            glUseProgram(0);
    }

    unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
    {
        unsigned int shader = glCreateShader(type);
        const char* src = source.c_str();
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);

        int result;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
        if (result == 0) // GL_FALSE
        {
            int length;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
            std::vector<char> message(length);
            glGetShaderInfoLog(shader, length, &length, message.data());
            NEXUS_CORE_ERROR("Failed to compile " + std::string(type == GL_VERTEX_SHADER ? "vertex" : "fragment") + " shader: " + std::string(message.data()));
            glDeleteShader(shader);
            return 0;
        }

        NEXUS_CORE_INFO("Compiled " + std::string(type == GL_VERTEX_SHADER ? "vertex" : "fragment") + " shader successfully");
        return shader;
    }

    unsigned int Shader::CreateShaderProgram(const std::string& vertexSource, const std::string& fragmentSource)
    {
        unsigned int program = glCreateProgram();
        unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexSource);
        unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

        glAttachShader(program, vs);
        glAttachShader(program, fs);
        glLinkProgram(program);

        int result;
        glGetProgramiv(program, GL_LINK_STATUS, &result);
        if (result == 0) // GL_FALSE
        {
            int length;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
            std::vector<char> message(length);
            glGetProgramInfoLog(program, length, &length, message.data());
            NEXUS_CORE_ERROR("Failed to link shader program: " + std::string(message.data()));
            glDeleteProgram(program);
            return 0;
        }

        glDeleteShader(vs);
        glDeleteShader(fs);

        NEXUS_CORE_INFO("Shader program '" + m_Name + "' created successfully");
        return program;
    }

    int Shader::GetUniformLocation(const std::string& name)
    {
        if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
            return m_UniformLocationCache[name];

        int location = glGetUniformLocation(m_RendererID, name.c_str());
        if (location == -1)
            NEXUS_CORE_WARN("Warning: uniform '" + name + "' doesn't exist!");

        m_UniformLocationCache[name] = location;
        return location;
    }

    void Shader::SetInt(const std::string& name, int value)
    {
        if (glUniform1i)
            glUniform1i(GetUniformLocation(name), value);
    }

    void Shader::SetFloat(const std::string& name, float value)
    {
        if (glUniform1f)
            glUniform1f(GetUniformLocation(name), value);
    }

    void Shader::SetFloat3(const std::string& name, const Vector3& value)
    {
        if (glUniform3f)
            glUniform3f(GetUniformLocation(name), value.x, value.y, value.z);
    }

    void Shader::SetMatrix4(const std::string& name, const Matrix4& value)
    {
        if (glUniformMatrix4fv)
            glUniformMatrix4fv(GetUniformLocation(name), 1, 0, value.Data());
    }

    Shader* Shader::Create(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource)
    {
        return new Shader(name, vertexSource, fragmentSource);
    }
}