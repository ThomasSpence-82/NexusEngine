#include "Core/Logger.h"
#include "Core/Window.h"
#include "Input/InputManager.h"
#include "Renderer/Shader.h"
#include "Renderer/Texture.h"
#include "Renderer/Camera.h"
#include "Renderer/Gizmo.h"
#include "Math/Vector3.h"
#include "Math/Matrix4.h"
#include <windows.h>
#include <GL/gl.h>

// 3D vertex and fragment shaders with MVP matrices and textures
const std::string vertexShaderSource = R"(
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Color;
layout(location = 2) in vec2 a_TexCoord;

uniform mat4 u_MVP;

out vec3 v_Color;
out vec2 v_TexCoord;

void main()
{
    v_Color = a_Color;
    v_TexCoord = a_TexCoord;
    gl_Position = u_MVP * vec4(a_Position, 1.0);
}
)";

const std::string fragmentShaderSource = R"(
#version 330 core

in vec3 v_Color;
in vec2 v_TexCoord;

uniform sampler2D u_Texture;

out vec4 FragColor;

void main()
{
    vec4 texColor = texture(u_Texture, v_TexCoord);
    FragColor = texColor * vec4(v_Color, 1.0);
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
    auto window = Nexus::Window::Create(Nexus::WindowProps("NexusEngine - 3D CUBE WITH SCREEN-SPACE GIZMO!", 1280, 720));

    if (!window)
    {
        NEXUS_CORE_ERROR("Failed to create window!");
        return -1;
    }

    NEXUS_INFO("=== 3D CUBE WITH PROFESSIONAL GIZMO ===");
    NEXUS_INFO("WASD/QE - Move camera, Arrow Keys - Look around, ESC - Exit");
    NEXUS_INFO("G - Toggle Gizmo | SPACE - Toggle Screen/World space");

    // Create shader, texture, camera, and gizmo
    auto shader = Nexus::Shader::Create("CubeShader", vertexShaderSource, fragmentShaderSource);
    auto texture = Nexus::Texture::Create("checkerboard");
    auto camera = new Nexus::Camera(45.0f, 1280.0f / 720.0f, 0.1f, 100.0f);
    auto gizmo = Nexus::Gizmo::Create();

    if (!shader)
    {
        NEXUS_CORE_ERROR("Failed to create shader!");
        return -1;
    }

    if (!gizmo)
    {
        NEXUS_CORE_ERROR("Failed to create gizmo!");
        return -1;
    }

    // Position camera to see the cube clearly
    camera->SetPosition(Nexus::Vector3(5.0f, 3.0f, -1.0f));

    // Cube vertex data: position (3) + color (3) + texture coords (2) = 8 floats per vertex
    float vertices[] = {
        // Front face
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f, // Bottom-left
         0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, // Bottom-right
         0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f, // Top-right
        -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  0.0f, 1.0f, // Top-left

        // Back face  
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,  1.0f, 0.0f, // Bottom-left
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,  0.0f, 0.0f, // Bottom-right
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f, // Top-right
        -0.5f,  0.5f, -0.5f,  0.5f, 0.5f, 0.5f,  1.0f, 1.0f, // Top-left
    };

    // Cube indices (2 triangles per face, 6 faces = 12 triangles = 36 indices)
    unsigned int indices[] = {
        // Front face
        0, 1, 2, 2, 3, 0,
        // Back face
        4, 5, 6, 6, 7, 4,
        // Left face
        7, 3, 0, 0, 4, 7,
        // Right face
        1, 5, 6, 6, 2, 1,
        // Top face
        3, 2, 6, 6, 7, 3,
        // Bottom face
        0, 1, 5, 5, 4, 0
    };

    // Load OpenGL functions (using your original working pattern)
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

    // Check if functions loaded successfully
    if (!glGenVertexArrays || !glBindVertexArray || !glGenBuffers || !glBindBuffer ||
        !glBufferData || !glEnableVertexAttribArray || !glVertexAttribPointer)
    {
        NEXUS_CORE_ERROR("Failed to load OpenGL buffer functions!");
        return -1;
    }

    NEXUS_INFO("OpenGL functions loaded! Gizmo will appear in top-right corner!");

    // Create VAO, VBO, and EBO
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // Upload vertex data
    glBindBuffer(0x8892, VBO); // GL_ARRAY_BUFFER
    glBufferData(0x8892, sizeof(vertices), vertices, 0x88E4); // GL_STATIC_DRAW

    // Upload index data
    glBindBuffer(0x8893, EBO); // GL_ELEMENT_ARRAY_BUFFER
    glBufferData(0x8893, sizeof(indices), indices, 0x88E4); // GL_STATIC_DRAW

    // Position attribute (location 0)
    glVertexAttribPointer(0, 3, 0x1406, 0, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Color attribute (location 1)
    glVertexAttribPointer(1, 3, 0x1406, 0, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Texture coordinate attribute (location 2)
    glVertexAttribPointer(2, 2, 0x1406, 0, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    NEXUS_INFO("Scene ready! Cube centered, gizmo in top-right corner!");

    // Enable depth testing
    glEnable(0x0B71); // GL_DEPTH_TEST

    // Scene state
    float cubeRotation = 0.0f;
    bool showGizmo = true;
    Nexus::Vector3 cubePosition = Nexus::Vector3::Zero;

    // Set gizmo to screen-space mode by default
    gizmo->SetScreenSpace(true);

    // Main loop
    while (!window->ShouldClose())
    {
        window->Update();

        // Camera controls
        const float moveSpeed = 3.0f * (1.0f / 60.0f);
        const float rotateSpeed = 1.5f * (1.0f / 60.0f);

        // Movement controls
        if (Nexus::InputManager::IsKeyDown(Nexus::KeyCode::W))
            camera->MoveForward(moveSpeed);
        if (Nexus::InputManager::IsKeyDown(Nexus::KeyCode::S))
            camera->MoveForward(-moveSpeed);
        if (Nexus::InputManager::IsKeyDown(Nexus::KeyCode::A))
            camera->MoveRight(-moveSpeed);
        if (Nexus::InputManager::IsKeyDown(Nexus::KeyCode::D))
            camera->MoveRight(moveSpeed);
        if (Nexus::InputManager::IsKeyDown(Nexus::KeyCode::Q))
            camera->MoveUp(moveSpeed);
        if (Nexus::InputManager::IsKeyDown(Nexus::KeyCode::E))
            camera->MoveUp(-moveSpeed);

        // Camera rotation controls
        if (Nexus::InputManager::IsKeyDown(Nexus::KeyCode::Left))
            camera->RotateYaw(-rotateSpeed);
        if (Nexus::InputManager::IsKeyDown(Nexus::KeyCode::Right))
            camera->RotateYaw(rotateSpeed);
        if (Nexus::InputManager::IsKeyDown(Nexus::KeyCode::Up))
            camera->RotatePitch(rotateSpeed);
        if (Nexus::InputManager::IsKeyDown(Nexus::KeyCode::Down))
            camera->RotatePitch(-rotateSpeed);

        // Gizmo controls
        if (Nexus::InputManager::IsKeyPressed(Nexus::KeyCode::G))
        {
            showGizmo = !showGizmo;
            NEXUS_INFO("Gizmo " + std::string(showGizmo ? "enabled" : "disabled"));
        }

        // Toggle between screen-space and world-space gizmo
        if (Nexus::InputManager::IsKeyPressed(Nexus::KeyCode::Space))
        {
            gizmo->SetScreenSpace(!gizmo->IsScreenSpace());
        }

        // Exit
        if (Nexus::InputManager::IsKeyPressed(Nexus::KeyCode::Escape))
            break;

        // Clear screen
        glClearColor(0.15f, 0.15f, 0.2f, 1.0f);
        glClear(0x00004100); // GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT

        // Animate cube rotation
        cubeRotation += 0.8f * (1.0f / 60.0f);

        // Create model matrix for cube
        Nexus::Matrix4 model = Nexus::Matrix4::Translate(cubePosition) *
            Nexus::Matrix4::RotateY(cubeRotation) *
            Nexus::Matrix4::RotateX(cubeRotation * 0.3f);

        // Create MVP matrix for cube
        Nexus::Matrix4 mvp = camera->GetViewProjectionMatrix() * model;

        // Render the textured cube
        shader->Bind();
        shader->SetMatrix4("u_MVP", mvp);
        shader->SetInt("u_Texture", 0);

        texture->Bind(0);
        glBindVertexArray(VAO);
        ::glDrawElements(0x0004, 36, 0x1405, nullptr); // GL_TRIANGLES, GL_UNSIGNED_INT

        // Render the gizmo
        if (showGizmo)
        {
            gizmo->Render(camera->GetViewProjectionMatrix(), cubePosition);
        }

        window->SwapBuffers();
    }

    // Cleanup
    delete gizmo;
    delete camera;
    delete texture;
    delete shader;
    Nexus::InputManager::Shutdown();
    delete window;
    NEXUS_CORE_INFO("3D Scene with Screen-Space Gizmo completed!");

    return 0;
}