#pragma once

#include "Math/Vector2.h"
#include <GLFW/glfw3.h>
#include <unordered_map>

namespace Nexus
{
    enum class KeyCode
    {
        // Letters
        A = 65, B = 66, C = 67, D = 68, E = 69, F = 70, G = 71, H = 72, I = 73, J = 74,
        K = 75, L = 76, M = 77, N = 78, O = 79, P = 80, Q = 81, R = 82, S = 83, T = 84,
        U = 85, V = 86, W = 87, X = 88, Y = 89, Z = 90,

        // Numbers
        Num0 = 48, Num1 = 49, Num2 = 50, Num3 = 51, Num4 = 52,
        Num5 = 53, Num6 = 54, Num7 = 55, Num8 = 56, Num9 = 57,

        // Special keys
        Space = 32,
        Escape = 256,
        Enter = 257,
        Tab = 258,
        Backspace = 259,
        Insert = 260,
        Delete = 261,
        Right = 262,
        Left = 263,
        Down = 264,
        Up = 265,

        // Function keys
        F1 = 290, F2 = 291, F3 = 292, F4 = 293, F5 = 294, F6 = 295,
        F7 = 296, F8 = 297, F9 = 298, F10 = 299, F11 = 300, F12 = 301,

        // Modifiers
        LeftShift = 340,
        LeftControl = 341,
        LeftAlt = 342,
        RightShift = 344,
        RightControl = 345,
        RightAlt = 346
    };

    enum class MouseButton
    {
        Left = 0,
        Right = 1,
        Middle = 2
    };

    class InputManager
    {
    public:
        // Initialize input system
        static void Initialize();
        static void Shutdown();

        // Update input state (call once per frame)
        static void Update();

        // Keyboard input
        static bool IsKeyPressed(KeyCode key);
        static bool IsKeyDown(KeyCode key);
        static bool IsKeyUp(KeyCode key);

        // Mouse input
        static bool IsMouseButtonPressed(MouseButton button);
        static bool IsMouseButtonDown(MouseButton button);
        static bool IsMouseButtonUp(MouseButton button);

        // Mouse position
        static Vector2 GetMousePosition();
        static Vector2 GetMouseDelta();

        // Mouse scroll
        static Vector2 GetMouseScroll();

        // Internal GLFW callbacks (don't call directly)
        static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
        static void MousePositionCallback(GLFWwindow* window, double xpos, double ypos);
        static void MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

    private:
        static std::unordered_map<int, bool> s_KeyStates;
        static std::unordered_map<int, bool> s_PreviousKeyStates;
        static std::unordered_map<int, bool> s_MouseButtonStates;
        static std::unordered_map<int, bool> s_PreviousMouseButtonStates;

        static Vector2 s_MousePosition;
        static Vector2 s_PreviousMousePosition;
        static Vector2 s_MouseScroll;

        static bool s_Initialized;
    };

    // Convenience functions
    inline bool IsKeyPressed(KeyCode key) { return InputManager::IsKeyPressed(key); }
    inline bool IsKeyDown(KeyCode key) { return InputManager::IsKeyDown(key); }
    inline bool IsKeyUp(KeyCode key) { return InputManager::IsKeyUp(key); }
    inline bool IsMouseButtonPressed(MouseButton button) { return InputManager::IsMouseButtonPressed(button); }
    inline Vector2 GetMousePosition() { return InputManager::GetMousePosition(); }
}
