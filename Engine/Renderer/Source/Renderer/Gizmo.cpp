#include "Renderer/Gizmo.h"
#include "Renderer/Shader.h"
#include "Core/Logger.h"
#include <windows.h>
#include <GL/gl.h>
#include <cmath>

namespace Nexus
{
    // OpenGL constants
    #define GL_ARRAY_BUFFER 0x8892
    #define GL_STATIC_DRAW 0x88E4
    #define GL_LINES 0x0001
    #define GL_TRIANGLES 0x0004
    #define GL_FLOAT 0x1406
    #define GL_FALSE 0
    #define GL_DEPTH_TEST 0x0B71

    // Commercial standard gizmo shader with fixed perspective
    const std::string gizmoVertexShader = R"(
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Color;

uniform mat4 u_MVP;
uniform int u_ScreenSpace;

out vec3 v_Color;

void main()
{
    if (u_ScreenSpace == 1) {
        // Fixed perspective - commercial standard (like Unity/Unreal)
        // Apply fixed rotation to ensure all 3 axes are always visible
        vec3 pos = a_Position;
        
        // Apply fixed 45-degree rotation around Y and slight X tilt
        float cosY = 0.707; // cos(45 degrees)
        float sinY = 0.707; // sin(45 degrees)
        float cosX = 0.95;  // cos(18 degrees) - slight downward tilt
        float sinX = 0.31;  // sin(18 degrees)
        
        // Rotate around Y first, then X
        vec3 rotated;
        rotated.x = pos.x * cosY + pos.z * sinY;
        rotated.z = -pos.x * sinY + pos.z * cosY;
        rotated.y = pos.y;
        
        vec3 final;
        final.x = rotated.x;
        final.y = rotated.y * cosX - rotated.z * sinX;
        final.z = rotated.y * sinX + rotated.z * cosX;
        
        // Position in top-right corner
        vec2 screenPos = final.xy * 0.12 + vec2(0.75, 0.75);
        gl_Position = vec4(screenPos, -0.9, 1.0);
    } else {
        gl_Position = u_MVP * vec4(a_Position, 1.0);
    }
    
    v_Color = a_Color;
}
)";

    const std::string gizmoFragmentShader = R"(
#version 330 core

in vec3 v_Color;
out vec4 FragColor;

void main()
{
    FragColor = vec4(v_Color, 1.0);
}
)";

    static void CheckGLError(const std::string& operation)
    {
        GLenum error = glGetError();
        if (error != GL_NO_ERROR)
        {
            NEXUS_CORE_ERROR("OpenGL Error in " + operation + ": 0x" + std::to_string(error));
        }
    }

    Gizmo::Gizmo()
        : m_VAO(0), m_VBO(0), m_Shader(nullptr), m_ScreenSpace(true)
    {
        Initialize();
    }

    Gizmo::~Gizmo()
    {
        // Load OpenGL functions for cleanup
        typedef void (APIENTRY* PFNGLDELETEVERTEXARRAYSPROC)(int n, const unsigned int* arrays);
        typedef void (APIENTRY* PFNGLDELETEBUFFERSPROC)(int n, const unsigned int* buffers);

        auto glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)wglGetProcAddress("glDeleteVertexArrays");
        auto glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)wglGetProcAddress("glDeleteBuffers");

        if (m_VAO && glDeleteVertexArrays)
        {
            glDeleteVertexArrays(1, &m_VAO);
        }

        if (m_VBO && glDeleteBuffers)
        {
            glDeleteBuffers(1, &m_VBO);
        }

        delete m_Shader;
        m_Shader = nullptr;
    }

    void Gizmo::Initialize()
    {
        // Create commercial standard gizmo shader
        m_Shader = Shader::Create("CommercialGizmoShader", gizmoVertexShader, gizmoFragmentShader);
        if (!m_Shader)
        {
            NEXUS_CORE_ERROR("Failed to create Commercial Gizmo shader");
            return;
        }

        // Commercial standard gizmo data - includes axes, arrows, and simple text indicators
        float axisLength = 1.0f;
        float arrowSize = 0.08f;
        float arrowPos = 0.85f;

        float gizmoVertices[] = {
            // Main axes
            // X-Axis (Red)
            0.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  // Origin
            axisLength, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  // X direction

            // Y-Axis (Green)
            0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,  // Origin
            0.0f, axisLength, 0.0f,  0.0f, 1.0f, 0.0f,  // Y direction

            // Z-Axis (Blue)
            0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f,  // Origin
            0.0f, 0.0f, axisLength,  0.0f, 0.0f, 1.0f,  // Z direction

            // Arrow indicators (simple line-based arrows)
            // X-axis arrow (red)
            arrowPos, -arrowSize, 0.0f,  1.0f, 0.0f, 0.0f,
            axisLength, 0.0f, 0.0f,      1.0f, 0.0f, 0.0f,
            arrowPos,  arrowSize, 0.0f,  1.0f, 0.0f, 0.0f,
            axisLength, 0.0f, 0.0f,      1.0f, 0.0f, 0.0f,

            // Y-axis arrow (green)
            -arrowSize, arrowPos, 0.0f,  0.0f, 1.0f, 0.0f,
            0.0f, axisLength, 0.0f,      0.0f, 1.0f, 0.0f,
             arrowSize, arrowPos, 0.0f,  0.0f, 1.0f, 0.0f,
            0.0f, axisLength, 0.0f,      0.0f, 1.0f, 0.0f,

            // Z-axis arrow (blue)
            0.0f, -arrowSize, arrowPos,  0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, axisLength,      0.0f, 0.0f, 1.0f,
            0.0f,  arrowSize, arrowPos,  0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, axisLength,      0.0f, 0.0f, 1.0f,

            // Simple letter indicators (line-based X, Y, Z)
            // X letter at end of red axis
            1.1f, -0.05f, 0.0f,  1.0f, 0.0f, 0.0f,
            1.2f,  0.05f, 0.0f,  1.0f, 0.0f, 0.0f,
            1.1f,  0.05f, 0.0f,  1.0f, 0.0f, 0.0f,
            1.2f, -0.05f, 0.0f,  1.0f, 0.0f, 0.0f,

            // Y letter at end of green axis (simple V + line)
            -0.025f, 1.1f, 0.0f,   0.0f, 1.0f, 0.0f,
            0.0f,    1.15f, 0.0f,  0.0f, 1.0f, 0.0f,
            0.025f,  1.1f, 0.0f,   0.0f, 1.0f, 0.0f,
            0.0f,    1.15f, 0.0f,  0.0f, 1.0f, 0.0f,
            0.0f,    1.15f, 0.0f,  0.0f, 1.0f, 0.0f,
            0.0f,    1.2f, 0.0f,   0.0f, 1.0f, 0.0f,

            // Z letter at end of blue axis (zigzag)
            -0.05f, 0.05f, 1.1f,   0.0f, 0.0f, 1.0f,
             0.05f, 0.05f, 1.1f,   0.0f, 0.0f, 1.0f,
             0.05f, 0.05f, 1.1f,   0.0f, 0.0f, 1.0f,
            -0.05f, -0.05f, 1.1f,  0.0f, 0.0f, 1.0f,
            -0.05f, -0.05f, 1.1f,  0.0f, 0.0f, 1.0f,
             0.05f, -0.05f, 1.1f,  0.0f, 0.0f, 1.0f
        };

        // Load OpenGL functions
        typedef void (APIENTRY* PFNGLGENVERTEXARRAYSPROC)(int n, unsigned int* arrays);
        typedef void (APIENTRY* PFNGLBINDVERTEXARRAYPROC)(unsigned int array);
        typedef void (APIENTRY* PFNGLGENBUFFERSPROC)(int n, unsigned int* buffers);
        typedef void (APIENTRY* PFNGLBINDBUFFERPROC)(unsigned int target, unsigned int buffer);
        typedef void (APIENTRY* PFNGLBUFFERDATAPROC)(unsigned int target, long long size, const void* data, unsigned int usage);
        typedef void (APIENTRY* PFNGLENABLEVERTEXATTRIBARRAYPROC)(unsigned int index);
        typedef void (APIENTRY* PFNGLVERTEXATTRIBPOINTERPROC)(unsigned int index, int size, unsigned int type, unsigned char normalized, int stride, const void* pointer);

        auto glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)wglGetProcAddress("glGenVertexArrays");
        auto glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)wglGetProcAddress("glBindVertexArray");
        auto glGenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
        auto glBindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
        auto glBufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
        auto glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glEnableVertexAttribArray");
        auto glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)wglGetProcAddress("glVertexAttribPointer");

        // Create single VAO for everything (axes + arrows + letters)
        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);

        glBindVertexArray(m_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(gizmoVertices), gizmoVertices, GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // Color attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
        CheckGLError("Create commercial standard gizmo");

        NEXUS_CORE_INFO("Commercial Standard Gizmo initialized: Fixed perspective + X,Y,Z labels");
    }

    void Gizmo::Render(const Matrix4& viewProjectionMatrix, const Vector3& position)
    {
        if (!m_Shader || m_VAO == 0)
        {
            NEXUS_CORE_WARN("Commercial Gizmo not properly initialized");
            return;
        }

        // Load OpenGL functions for rendering
        typedef void (APIENTRY* PFNGLBINDVERTEXARRAYPROC)(unsigned int array);
        auto glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)wglGetProcAddress("glBindVertexArray");

        // Disable depth testing for screen-space gizmo
        bool wasDepthEnabled = glIsEnabled(GL_DEPTH_TEST);
        if (m_ScreenSpace)
        {
            glDisable(GL_DEPTH_TEST);
        }

        m_Shader->Bind();

        // Set uniforms
        m_Shader->SetInt("u_ScreenSpace", m_ScreenSpace ? 1 : 0);

        if (!m_ScreenSpace)
        {
            // World-space rendering
            Matrix4 model = Matrix4::Translate(position);
            Matrix4 mvp = viewProjectionMatrix * model;
            m_Shader->SetMatrix4("u_MVP", mvp);
        }
        else
        {
            // Screen-space with fixed perspective (handled in vertex shader)
            m_Shader->SetMatrix4("u_MVP", Matrix4::Identity());
        }

        // Render everything as lines
        glBindVertexArray(m_VAO);
        ::glDrawArrays(GL_LINES, 0, 30); // 3 axes + 6 arrow lines + 12 letter lines = 21 lines = 42 vertices

        glBindVertexArray(0);
        m_Shader->Unbind();

        // Restore depth testing
        if (m_ScreenSpace && wasDepthEnabled)
        {
            glEnable(GL_DEPTH_TEST);
        }

        CheckGLError("Render commercial standard gizmo");
    }

    void Gizmo::SetScreenSpace(bool screenSpace)
    {
        m_ScreenSpace = screenSpace;
        if (screenSpace)
        {
            NEXUS_CORE_INFO("Gizmo: Commercial screen-space mode (fixed perspective - all 3 axes always visible)");
        }
        else
        {
            NEXUS_CORE_INFO("Gizmo: World-space mode (follows object)");
        }
    }

    Gizmo* Gizmo::Create()
    {
        return new Gizmo();
    }
}