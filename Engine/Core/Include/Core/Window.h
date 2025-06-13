#pragma once

#include <GLFW/glfw3.h>
#include <string>

namespace Nexus
{
    struct WindowProps
    {
        std::string Title;
        unsigned int Width;
        unsigned int Height;

        WindowProps(const std::string& title = "NexusEngine",
            unsigned int width = 1280,
            unsigned int height = 720)
            : Title(title), Width(width), Height(height) {
        }
    };

    class Window
    {
    public:
        Window(const WindowProps& props = WindowProps());
        ~Window();

        void Update();
        void SwapBuffers();

        unsigned int GetWidth() const { return m_Data.Width; }
        unsigned int GetHeight() const { return m_Data.Height; }

        bool ShouldClose() const;
        GLFWwindow* GetNativeWindow() const { return m_Window; }

        static Window* Create(const WindowProps& props = WindowProps());

    private:
        bool Init(const WindowProps& props);
        void Shutdown();

    private:
        GLFWwindow* m_Window;

        struct WindowData
        {
            std::string Title;
            unsigned int Width, Height;
        };

        WindowData m_Data;
    };
}
