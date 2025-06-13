#include "Core/Logger.h"
#include "Core/Window.h"
#include "Input/InputManager.h"
#include "Renderer/Shader.h"
#include "Math/Vector3.h"
#include <windows.h>
#include <GL/gl.h>

// Simple vertex and fragment shaders for our triangle
const std::string vertexShaderSource = R"(
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Color;

out vec3 v_Color;

void main()
{
    v_Color = a_Color;
    gl_Position = vec4(a_Position, 1.0);
}
)";

const std::string fragmentShaderSource = R"(
#version 330 core

in vec3 v_Color;
out vec4 FragColor;

void main()
{
    FragColor = vec4(v_Color, 1.0);
}
)";

int main()
{
    // Initialize the logger
    Nexus::Logger::Initialize();
    NEXUS_CORE_INFO("NexusEngine starting...");

    // Initialize input system
    Nexus::InputManager::Initialize();

    // Create window
    auto window = Nexus::Window::Create(Nexus::WindowProps("NexusEngine - THE LEGENDARY TRIANGLE!", 1280, 720));

    if (!window)
    {
        NEXUS_CORE_ERROR("Failed to create window!");
        return -1;
    }

    NEXUS_INFO("=== TRIANGLE RENDERING TEST ===");
    NEXUS_INFO("Preparing to render the legendary triangle!");

    // Create our first shader
    auto shader = Nexus::Shader::Create("TriangleShader", vertexShaderSource, fragmentShaderSource);

    if (!shader)
    {
        NEXUS_CORE_ERROR("Failed to create shader!");
        return -1;
    }

    NEXUS_INFO("Shader created successfully! Ready to render triangle!");

    // Triangle vertex data (position + color)
    float vertices[] = {
        // Position        // Color
        -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // Bottom left - Red
         0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // Bottom right - Green  
         0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // Top center - Blue
    };

    // Load OpenGL buffer functions (except glDrawArrays which is core OpenGL 1.1)
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

    // Check if functions loaded successfully (glDrawArrays is core OpenGL 1.1, always available)
    if (!glGenVertexArrays || !glBindVertexArray || !glGenBuffers || !glBindBuffer ||
        !glBufferData || !glEnableVertexAttribArray || !glVertexAttribPointer)
    {
        NEXUS_CORE_ERROR("Failed to load OpenGL buffer functions!");
        NEXUS_CORE_ERROR("glGenVertexArrays: " + std::string(glGenVertexArrays ? "OK" : "FAILED"));
        NEXUS_CORE_ERROR("glBindVertexArray: " + std::string(glBindVertexArray ? "OK" : "FAILED"));
        NEXUS_CORE_ERROR("glGenBuffers: " + std::string(glGenBuffers ? "OK" : "FAILED"));
        NEXUS_CORE_ERROR("glBindBuffer: " + std::string(glBindBuffer ? "OK" : "FAILED"));
        NEXUS_CORE_ERROR("glBufferData: " + std::string(glBufferData ? "OK" : "FAILED"));
        NEXUS_CORE_ERROR("glEnableVertexAttribArray: " + std::string(glEnableVertexAttribArray ? "OK" : "FAILED"));
        NEXUS_CORE_ERROR("glVertexAttribPointer: " + std::string(glVertexAttribPointer ? "OK" : "FAILED"));
        return -1;
    }

    NEXUS_INFO("OpenGL buffer functions loaded successfully!");
    NEXUS_INFO("Using core OpenGL glDrawArrays function");

    // Generate OpenGL buffers
    unsigned int VAO, VBO;

    // Create VAO and VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    NEXUS_INFO("VAO and VBO created: VAO=" + std::to_string(VAO) + " VBO=" + std::to_string(VBO));

    glBindVertexArray(VAO);
    glBindBuffer(0x8892, VBO); // GL_ARRAY_BUFFER
    glBufferData(0x8892, sizeof(vertices), vertices, 0x88E4); // GL_STATIC_DRAW

    // Position attribute (location 0)
    glVertexAttribPointer(0, 3, 0x1406, 0, 6 * sizeof(float), (void*)0); // GL_FLOAT
    glEnableVertexAttribArray(0);

    // Color attribute (location 1)  
    glVertexAttribPointer(1, 3, 0x1406, 0, 6 * sizeof(float), (void*)(3 * sizeof(float))); // GL_FLOAT
    glEnableVertexAttribArray(1);

    NEXUS_INFO("Triangle vertex data setup complete!");
    NEXUS_INFO("Press ESC to exit - Starting render loop...");

    // Main loop
    while (!window->ShouldClose())
    {
        // Update input first
        window->Update();

        // Close with ESC
        if (Nexus::InputManager::IsKeyPressed(Nexus::KeyCode::Escape))
        {
            NEXUS_INFO("ESC pressed - closing application!");
            break;
        }

        // Clear screen to dark background
        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Bind our shader and render THE TRIANGLE!
        shader->Bind();
        glBindVertexArray(VAO);
        ::glDrawArrays(0x0004, 0, 3); // Use core OpenGL function - GL_TRIANGLES
        shader->Unbind();

        // Swap buffers
        window->SwapBuffers();
    }

    // Cleanup
    delete shader;
    Nexus::InputManager::Shutdown();
    delete window;
    NEXUS_CORE_INFO("Triangle test completed!");
    NEXUS_CORE_INFO("NexusEngine shutting down.");

    return 0;
}