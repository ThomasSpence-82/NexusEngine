-- NexusEngine Root Build Configuration
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

-- Engine Core
project "Core"
    location "Engine/Core"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"

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

-- Runtime Application
project "Runtime"
    location "Runtime"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"

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
        "Engine/Core/Include"
    }

    links
    {
        "Core"
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