#include "Input/InputManager.h"
#include "Core/Logger.h"
#include <GLFW/glfw3.h>

namespace Nexus
{
    // Static member definitions
    std::unordered_map<int, bool> InputManager::s_KeyStates;
    std::unordered_map<int, bool> InputManager::s_PreviousKeyStates;
    std::unordered_map<int, bool> InputManager::s_MouseButtonStates;
    std::unordered_map<int, bool> InputManager::s_PreviousMouseButtonStates;

    Vector2 InputManager::s_MousePosition;
    Vector2 InputManager::s_PreviousMousePosition;
    Vector2 InputManager::s_MouseScroll;

    bool InputManager::s_Initialized = false;

    void InputManager::Initialize()
    {
        if (s_Initialized)
            return;

        NEXUS_CORE_INFO("Input Manager initialized");
        s_Initialized = true;
    }

    void InputManager::Shutdown()
    {
        if (!s_Initialized)
            return;

        s_KeyStates.clear();
        s_PreviousKeyStates.clear();
        s_MouseButtonStates.clear();
        s_PreviousMouseButtonStates.clear();

        NEXUS_CORE_INFO("Input Manager shutdown");
        s_Initialized = false;
    }

    void InputManager::Update()
    {
        if (!s_Initialized)
            return;

        // Update previous states
        s_PreviousKeyStates = s_KeyStates;
        s_PreviousMouseButtonStates = s_MouseButtonStates;
        s_PreviousMousePosition = s_MousePosition;

        // Reset scroll delta
        s_MouseScroll = Vector2::Zero;
    }

    bool InputManager::IsKeyPressed(KeyCode key)
    {
        int keyCode = static_cast<int>(key);
        return s_KeyStates[keyCode] && !s_PreviousKeyStates[keyCode];
    }

    bool InputManager::IsKeyDown(KeyCode key)
    {
        int keyCode = static_cast<int>(key);
        return s_KeyStates[keyCode];
    }

    bool InputManager::IsKeyUp(KeyCode key)
    {
        int keyCode = static_cast<int>(key);
        return !s_KeyStates[keyCode] && s_PreviousKeyStates[keyCode];
    }

    bool InputManager::IsMouseButtonPressed(MouseButton button)
    {
        int buttonCode = static_cast<int>(button);
        return s_MouseButtonStates[buttonCode] && !s_PreviousMouseButtonStates[buttonCode];
    }

    bool InputManager::IsMouseButtonDown(MouseButton button)
    {
        int buttonCode = static_cast<int>(button);
        return s_MouseButtonStates[buttonCode];
    }

    bool InputManager::IsMouseButtonUp(MouseButton button)
    {
        int buttonCode = static_cast<int>(button);
        return !s_MouseButtonStates[buttonCode] && s_PreviousMouseButtonStates[buttonCode];
    }

    Vector2 InputManager::GetMousePosition()
    {
        return s_MousePosition;
    }

    Vector2 InputManager::GetMouseDelta()
    {
        return s_MousePosition - s_PreviousMousePosition;
    }

    Vector2 InputManager::GetMouseScroll()
    {
        return s_MouseScroll;
    }

    // GLFW Callbacks - Clean version without debug spam
    void InputManager::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if (action == GLFW_PRESS)
        {
            s_KeyStates[key] = true;
        }
        else if (action == GLFW_RELEASE)
        {
            s_KeyStates[key] = false;
        }
    }

    void InputManager::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
    {
        if (action == GLFW_PRESS)
        {
            s_MouseButtonStates[button] = true;
        }
        else if (action == GLFW_RELEASE)
        {
            s_MouseButtonStates[button] = false;
        }
    }

    void InputManager::MousePositionCallback(GLFWwindow* window, double xpos, double ypos)
    {
        s_MousePosition.x = static_cast<float>(xpos);
        s_MousePosition.y = static_cast<float>(ypos);
    }

    void InputManager::MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
    {
        s_MouseScroll.x = static_cast<float>(xoffset);
        s_MouseScroll.y = static_cast<float>(yoffset);
    }
}