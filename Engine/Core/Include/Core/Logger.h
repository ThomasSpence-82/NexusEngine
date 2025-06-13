#pragma once

#include <iostream>
#include <string>

namespace Nexus
{
    class Logger
    {
    public:
        static void Initialize();
        
        static void CoreInfo(const std::string& message);
        static void CoreWarn(const std::string& message);
        static void CoreError(const std::string& message);
        
        static void Info(const std::string& message);
        static void Warn(const std::string& message);
        static void Error(const std::string& message);
    };
}

// Core logging macros
#define NEXUS_CORE_INFO(msg)     ::Nexus::Logger::CoreInfo(msg)
#define NEXUS_CORE_WARN(msg)     ::Nexus::Logger::CoreWarn(msg)
#define NEXUS_CORE_ERROR(msg)    ::Nexus::Logger::CoreError(msg)

// Client logging macros  
#define NEXUS_INFO(msg)          ::Nexus::Logger::Info(msg)
#define NEXUS_WARN(msg)          ::Nexus::Logger::Warn(msg)
#define NEXUS_ERROR(msg)         ::Nexus::Logger::Error(msg)