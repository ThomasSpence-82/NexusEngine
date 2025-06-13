#include "Core/Logger.h"
#include <iostream>

namespace Nexus
{
    void Logger::Initialize()
    {
        std::cout << "[NEXUS] Logger initialized" << std::endl;
    }
    
    void Logger::CoreInfo(const std::string& message)
    {
        std::cout << "[CORE INFO] " << message << std::endl;
    }
    
    void Logger::CoreWarn(const std::string& message)
    {
        std::cout << "[CORE WARN] " << message << std::endl;
    }
    
    void Logger::CoreError(const std::string& message)
    {
        std::cout << "[CORE ERROR] " << message << std::endl;
    }
    
    void Logger::Info(const std::string& message)
    {
        std::cout << "[INFO] " << message << std::endl;
    }
    
    void Logger::Warn(const std::string& message)
    {
        std::cout << "[WARN] " << message << std::endl;
    }
    
    void Logger::Error(const std::string& message)
    {
        std::cout << "[ERROR] " << message << std::endl;
    }
}