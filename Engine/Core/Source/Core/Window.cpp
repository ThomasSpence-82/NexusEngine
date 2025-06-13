#include "Core/Window.h"
#include "Core/Logger.h"
#include "Input/InputManager.h"
#include <windows.h>
#include <GL/gl.h>
#include <vector>
#include <string>

// Additional OpenGL constants
#define GL_SHADING_LANGUAGE_VERSION       0x8B8C
#define GL_MAX_TEXTURE_SIZE               0x0D33

namespace Nexus
{
    static bool s_GLFWInitialized = false;

    static void GLFWErrorCallback(int error, const char* description)
    {
        NEXUS_CORE_ERROR("GLFW Error (" + std::to_string(error) + "): " + description);
    }

    // Clean GPU name extraction
    std::string ExtractGPUName(const std::string& deviceString)
    {
        // Remove common prefixes and clean up the name
        std::string cleaned = deviceString;
        
        // Remove manufacturer prefixes
        size_t pos = cleaned.find("Intel(R) ");
        if (pos != std::string::npos) cleaned = cleaned.substr(pos + 9);
        
        pos = cleaned.find("NVIDIA ");
        if (pos != std::string::npos) cleaned = cleaned.substr(pos + 7);
        
        pos = cleaned.find("AMD ");
        if (pos != std::string::npos) cleaned = cleaned.substr(pos + 4);
        
        return cleaned;
    }

    // Function to detect ALL graphics adapters with clean ASCII output
    void DetectAllGraphicsAdapters()
    {
        NEXUS_CORE_INFO("=== System Graphics Hardware ===");
        
        DISPLAY_DEVICEA displayDevice;
        displayDevice.cb = sizeof(displayDevice);
        
        std::vector<std::string> activeGPUs;
        std::vector<std::string> inactiveGPUs;
        
        for (DWORD deviceIndex = 0; ; deviceIndex++)
        {
            if (!EnumDisplayDevicesA(NULL, deviceIndex, &displayDevice, 0))
                break;
                
            std::string gpuName = ExtractGPUName(std::string(displayDevice.DeviceString));
            
            if (displayDevice.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP)
            {
                if (displayDevice.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)
                {
                    activeGPUs.push_back(gpuName + " (Primary)");
                }
                else
                {
                    activeGPUs.push_back(gpuName);
                }
            }
            else
            {
                // Only add if we haven't seen this GPU already
                bool alreadyListed = false;
                for (const auto& gpu : inactiveGPUs)
                {
                    if (gpu.find(gpuName) != std::string::npos)
                    {
                        alreadyListed = true;
                        break;
                    }
                }
                if (!alreadyListed)
                {
                    inactiveGPUs.push_back(gpuName + " (Available)");
                }
            }
        }
        
        // Display results with simple ASCII
        NEXUS_CORE_INFO("Active Graphics:");
        for (const auto& gpu : activeGPUs)
        {
            NEXUS_CORE_INFO("  [ACTIVE] " + gpu);
        }
        
        if (!inactiveGPUs.empty())
        {
            NEXUS_CORE_INFO("Available Graphics:");
            for (const auto& gpu : inactiveGPUs)
            {
                NEXUS_CORE_INFO("  [AVAIL]  " + gpu);
            }
        }
        
        NEXUS_CORE_INFO("Total GPUs Found: " + std::to_string(activeGPUs.size() + inactiveGPUs.size()));
    }

    Window* Window::Create(const WindowProps& props)
    {
        return new Window(props);
    }

    Window::Window(const WindowProps& props)
        : m_Window(nullptr)
    {
        Init(props);
    }

    Window::~Window()
    {
        Shutdown();
    }

    bool Window::Init(const WindowProps& props)
    {
        m_Data.Title = props.Title;
        m_Data.Width = props.Width;
        m_Data.Height = props.Height;

        NEXUS_CORE_INFO("Initializing NexusEngine Graphics System");

        // Detect ALL graphics hardware first
        DetectAllGraphicsAdapters();

        if (!s_GLFWInitialized)
        {
            int success = glfwInit();
            if (!success)
            {
                NEXUS_CORE_ERROR("Could not initialize GLFW!");
                return false;
            }
            glfwSetErrorCallback(GLFWErrorCallback);
            s_GLFWInitialized = true;
        }

        // Set OpenGL version to 3.3 Core
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, 
                                   m_Data.Title.c_str(), nullptr, nullptr);
        
        if (!m_Window)
        {
            NEXUS_CORE_ERROR("Failed to create GLFW window!");
            return false;
        }

        glfwMakeContextCurrent(m_Window);

        // Setup input callbacks
        glfwSetWindowUserPointer(m_Window, this);
        glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            InputManager::KeyCallback(window, key, scancode, action, mods);
        });
        glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
        {
            InputManager::MouseButtonCallback(window, button, action, mods);
        });
        glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos)
        {
            InputManager::MousePositionCallback(window, xpos, ypos);
        });
        glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xoffset, double yoffset)
        {
            InputManager::MouseScrollCallback(window, xoffset, yoffset);
        });

        // Clean OpenGL info
        NEXUS_CORE_INFO("=== Active OpenGL Context ===");
        std::string vendor = (char*)glGetString(GL_VENDOR);
        std::string renderer = (char*)glGetString(GL_RENDERER);
        std::string version = (char*)glGetString(GL_VERSION);
        
        NEXUS_CORE_INFO("Currently Using: " + ExtractGPUName(renderer));
        NEXUS_CORE_INFO("OpenGL Version: " + version);
        
        const char* glslVersion = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
        if (glslVersion)
        {
            NEXUS_CORE_INFO("GLSL Version: " + std::string(glslVersion));
        }

        GLint maxTextureSize;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
        NEXUS_CORE_INFO("Max Texture Size: " + std::to_string(maxTextureSize) + "x" + std::to_string(maxTextureSize));

        // Performance recommendation
        if (renderer.find("Intel") != std::string::npos)
        {
            NEXUS_CORE_WARN("PERFORMANCE TIP: Currently using integrated graphics");
            NEXUS_CORE_WARN("To use discrete GPU: NVIDIA Control Panel > Program Settings > Add Runtime.exe");
        }
        else if (renderer.find("NVIDIA") != std::string::npos || renderer.find("GeForce") != std::string::npos)
        {
            NEXUS_CORE_INFO("EXCELLENT: Using discrete NVIDIA graphics for optimal performance");
        }

        glfwSwapInterval(1);
        NEXUS_CORE_INFO("Window Resolution: " + std::to_string(props.Width) + "x" + std::to_string(props.Height) + " @ 60Hz V-Sync");
        NEXUS_CORE_INFO("Input callbacks registered successfully");

        return true;
    }

    void Window::Shutdown()
    {
        if (m_Window)
        {
            glfwDestroyWindow(m_Window);
            m_Window = nullptr;
        }
    }

    void Window::Update()
    {
        glfwPollEvents();
        InputManager::Update();  // Update input state each frame
    }

    void Window::SwapBuffers()
    {
        glfwSwapBuffers(m_Window);
    }

    bool Window::ShouldClose() const
    {
        return glfwWindowShouldClose(m_Window);
    }
}