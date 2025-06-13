#include "Core/Logger.h"
#include "Core/Window.h"
#include <windows.h>
#include <GL/gl.h>

int main()
{
    // Initialize the logger
    Nexus::Logger::Initialize();
    NEXUS_CORE_INFO("NexusEngine starting...");

    // Create window
    auto window = Nexus::Window::Create(Nexus::WindowProps("NexusEngine - Session 002", 1280, 720));

    if (!window)
    {
        NEXUS_CORE_ERROR("Failed to create window!");
        return -1;
    }

    NEXUS_INFO("Window created successfully!");

    // Main loop
    while (!window->ShouldClose())
    {
        // Clear screen to dark blue
        glClearColor(0.1f, 0.1f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Update window
        window->Update();
        window->SwapBuffers();
    }

    // Cleanup
    delete window;
    NEXUS_CORE_INFO("NexusEngine shutting down.");

    return 0;
}