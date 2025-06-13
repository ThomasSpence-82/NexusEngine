-- NexusEngine Root Build Configuration
-- Session 003 - Added Math and Input modules

workspace "NexusEngine"
    architecture "x64"
    startproject "Runtime"
    
    configurations
    {
        "Debug",
        "Release"
    }
    
    platforms
    {
        "Win64"
    }

-- Global output directories
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder
IncludeDir = {}
IncludeDir["GLFW"] = "ThirdParty/GLFW/include"
IncludeDir["GLM"] = "ThirdParty/GLM"

-- Build GLFW as a static library
project "GLFW"
    location "ThirdParty/GLFW"
    kind "StaticLib"
    language "C"
    staticruntime "off"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.location}/include/GLFW/glfw3.h",
        "%{prj.location}/include/GLFW/glfw3native.h",
        "%{prj.location}/src/glfw_config.h",
        "%{prj.location}/src/context.c",
        "%{prj.location}/src/init.c",
        "%{prj.location}/src/input.c",
        "%{prj.location}/src/monitor.c",
        "%{prj.location}/src/vulkan.c",
        "%{prj.location}/src/window.c"
    }

    filter "system:windows"
        systemversion "latest"
        
        files
        {
            "%{prj.location}/src/win32_init.c",
            "%{prj.location}/src/win32_joystick.c",
            "%{prj.location}/src/win32_monitor.c",
            "%{prj.location}/src/win32_time.c",
            "%{prj.location}/src/win32_thread.c",
            "%{prj.location}/src/win32_window.c",
            "%{prj.location}/src/wgl_context.c",
            "%{prj.location}/src/egl_context.c",
            "%{prj.location}/src/osmesa_context.c"
        }

        defines 
        { 
            "_GLFW_WIN32",
            "_CRT_SECURE_NO_WARNINGS"
        }

-- Math Library
project "Math"
    location "Engine/Math"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.location}/Include/**.h",
        "%{prj.location}/Source/**.cpp"
    }

    includedirs
    {
        "%{prj.location}/Include"
    }

    filter "system:windows"
        systemversion "latest"
        defines "NEXUS_PLATFORM_WINDOWS"

    filter "configurations:Debug"
        defines "NEXUS_DEBUG"
        symbols "on"

    filter "configurations:Release"
        defines "NEXUS_RELEASE"
        optimize "on"

-- Input System
project "Input"
    location "Engine/Input"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.location}/Include/**.h",
        "%{prj.location}/Source/**.cpp"
    }

    includedirs
    {
        "%{prj.location}/Include",
        "Engine/Math/Include",
        "Engine/Core/Include",
        "%{IncludeDir.GLFW}"
    }

    links
    {
        "Math"
    }

    filter "system:windows"
        systemversion "latest"
        defines "NEXUS_PLATFORM_WINDOWS"

    filter "configurations:Debug"
        defines "NEXUS_DEBUG"
        symbols "on"

    filter "configurations:Release"
        defines "NEXUS_RELEASE"
        optimize "on"

-- Engine Core
project "Core"
    location "Engine/Core"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.location}/Include/**.h",
        "%{prj.location}/Source/**.cpp"
    }

    defines
    {
        "_CRT_SECURE_NO_WARNINGS",
        "GLFW_INCLUDE_NONE"
    }

    includedirs
    {
        "%{prj.location}/Include",
        "Engine/Math/Include",
        "Engine/Input/Include",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.GLM}"
    }

    links
    {
        "Math",
        "Input",
        "GLFW",
        "opengl32.lib"
    }

    filter "system:windows"
        systemversion "latest"
        defines "NEXUS_PLATFORM_WINDOWS"

    filter "configurations:Debug"
        defines "NEXUS_DEBUG"
        symbols "on"

    filter "configurations:Release"
        defines "NEXUS_RELEASE"
        optimize "on"

-- Runtime Application
project "Runtime"
    location "Runtime"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.location}/Include/**.h",
        "%{prj.location}/Source/**.cpp"
    }

    includedirs
    {
        "%{prj.location}/Include",
        "Engine/Core/Include",
        "Engine/Math/Include",
        "Engine/Input/Include",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.GLM}"
    }

    links
    {
        "Core",
        "Math",
        "Input"
    }

    filter "system:windows"
        systemversion "latest"
        defines "NEXUS_PLATFORM_WINDOWS"

    filter "configurations:Debug"
        defines "NEXUS_DEBUG"
        symbols "on"

    filter "configurations:Release"
        defines "NEXUS_RELEASE"
        optimize "on"