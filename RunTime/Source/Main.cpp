#include "Core/Logger.h"

int main()
{
    // Initialize the logger
    Nexus::Logger::Initialize();
    
    // Test our logging system
    NEXUS_CORE_INFO("NexusEngine starting...");
    NEXUS_INFO("Hello from NexusEngine!");
    NEXUS_CORE_INFO("Everything working correctly!");
    
    // Keep the window open
    std::cout << "\nPress Enter to exit...";
    std::cin.get();
    
    NEXUS_CORE_INFO("NexusEngine shutting down.");
    
    return 0;
}