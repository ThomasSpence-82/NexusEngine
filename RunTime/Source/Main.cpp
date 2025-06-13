#include "Core/Logger.h"
#include "Core/Window.h"
#include "Input/InputManager.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Matrix4.h"
#include <windows.h>
#include <GL/gl.h>

int main()
{
    // Initialize the logger
    Nexus::Logger::Initialize();
    NEXUS_CORE_INFO("NexusEngine starting...");

    // Initialize input system
    Nexus::InputManager::Initialize();

    // Create window
    auto window = Nexus::Window::Create(Nexus::WindowProps("NexusEngine - Session 003 Input Test", 1280, 720));

    if (!window)
    {
        NEXUS_CORE_ERROR("Failed to create window!");
        return -1;
    }

    NEXUS_INFO("=== INPUT TEST READY ===");
    NEXUS_INFO("Click the BLUE WINDOW first to give it focus!");
    NEXUS_INFO("Then try: WASD, SPACE, ESC, Mouse clicks");
    NEXUS_INFO("Window will flash colors when input works!");

    // Main loop - DEBUG VERSION
    bool inputTestActive = true;
    int frameCount = 0;

    NEXUS_INFO("Input callbacks working! Testing main loop logic...");

    while (!window->ShouldClose() && inputTestActive)
    {
        frameCount++;

        // Update window and input FIRST
        window->Update();

        // DEBUG: Check if we can detect any key states
        bool wDown = Nexus::InputManager::IsKeyDown(Nexus::KeyCode::W);
        bool wPressed = Nexus::InputManager::IsKeyPressed(Nexus::KeyCode::W);
        bool escDown = Nexus::InputManager::IsKeyDown(Nexus::KeyCode::Escape);
        bool escPressed = Nexus::InputManager::IsKeyPressed(Nexus::KeyCode::Escape);

        if (wDown || wPressed)
        {
            NEXUS_INFO("DEBUG: W state - Down=" + std::to_string(wDown) + " Pressed=" + std::to_string(wPressed));
        }

        if (escDown || escPressed)
        {
            NEXUS_INFO("DEBUG: ESC state - Down=" + std::to_string(escDown) + " Pressed=" + std::to_string(escPressed));
        }

        // Try IsKeyDown instead of IsKeyPressed for now
        if (Nexus::InputManager::IsKeyDown(Nexus::KeyCode::W))
        {
            NEXUS_INFO("*** W DOWN - GREEN FLASH! ***");
            glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
        }
        else if (Nexus::InputManager::IsKeyDown(Nexus::KeyCode::A))
        {
            NEXUS_INFO("*** A DOWN - YELLOW FLASH! ***");
            glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
        }
        else if (Nexus::InputManager::IsKeyDown(Nexus::KeyCode::S))
        {
            NEXUS_INFO("*** S DOWN - RED FLASH! ***");
            glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
        }
        else if (Nexus::InputManager::IsKeyDown(Nexus::KeyCode::D))
        {
            NEXUS_INFO("*** D DOWN - BLUE FLASH! ***");
            glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
        }
        else if (Nexus::InputManager::IsKeyDown(Nexus::KeyCode::Space))
        {
            NEXUS_INFO("*** SPACE DOWN - PURPLE FLASH! ***");
            glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
        }
        else if (Nexus::InputManager::IsKeyDown(Nexus::KeyCode::Escape))
        {
            NEXUS_INFO("*** ESC DOWN - EXITING! ***");
            inputTestActive = false;
        }
        else
        {
            glClearColor(0.1f, 0.1f, 0.3f, 1.0f);
        }

        // Test mouse too
        if (Nexus::InputManager::IsMouseButtonDown(Nexus::MouseButton::Left))
        {
            Nexus::Vector2 mousePos = Nexus::InputManager::GetMousePosition();
            NEXUS_INFO("*** LEFT MOUSE DOWN at: " + mousePos.ToString() + " ***");
            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        }

        glClear(GL_COLOR_BUFFER_BIT);
        window->SwapBuffers();
    }

    // Cleanup
    Nexus::InputManager::Shutdown();
    delete window;
    NEXUS_CORE_INFO("Input test completed!");
    NEXUS_CORE_INFO("NexusEngine shutting down.");

    return 0;
}
