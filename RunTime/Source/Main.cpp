#include "Core/Logger.h"
#include "Core/Window.h"
#include "Scene/ECS/Registry.h"
#include "Scene/ECS/Components/Transform.h"
#include "Scene/ECS/Components/Name.h"
#include "Scene/ECS/Components/Camera.h"
#include "Scene/ECS/Components/Light.h"
#include "Scene/ECS/Components/MeshRenderer.h"

void TestECSSystem()
{
    NEXUS_CORE_INFO("=== Testing ECS System ===");

    // Create a registry
    Nexus::Registry registry;

    // Create some entities
    auto camera = registry.CreateEntity();
    auto cube = registry.CreateEntity();
    auto light = registry.CreateEntity();

    // Add components to camera
    camera.AddComponent<Nexus::Name>("Main Camera");
    camera.AddComponent<Nexus::Transform>(Nexus::Vector3(0, 0, 5));
    auto& cameraComp = camera.AddComponent<Nexus::Camera>();
    cameraComp.isPrimary = true;

    // Add components to cube
    cube.AddComponent<Nexus::Name>("Test Cube");
    cube.AddComponent<Nexus::Tag>("Geometry");
    auto& cubeTransform = cube.AddComponent<Nexus::Transform>();
    cubeTransform.SetPosition(Nexus::Vector3(0, 0, 0));
    cubeTransform.SetScale(Nexus::Vector3(1, 1, 1));
    cube.AddComponent<Nexus::MeshRenderer>("cube.obj", "default.mat");

    // Add components to light
    light.AddComponent<Nexus::Name>("Main Light");
    auto& lightTransform = light.AddComponent<Nexus::Transform>(Nexus::Vector3(2, 4, 2));
    lightTransform.SetEulerAngles(Nexus::Vector3(-45, 30, 0) * (3.14159f / 180.0f)); // Convert to radians
    light.AddComponent<Nexus::Light>(Nexus::Light::CreateDirectional());

    // Test component queries
    NEXUS_CORE_INFO("Testing component queries...");

    // Get all entities with Transform components
    auto view = registry.GetView<Nexus::Transform>();
    int transformCount = 0;
    for (auto [entity, transform] : view)
    {
        if (entity.HasComponent<Nexus::Name>())
        {
            auto& name = entity.GetComponent<Nexus::Name>();
            NEXUS_CORE_INFO("Entity: " + name.name + " at position: " + transform.position.ToString());
        }
        transformCount++;
    }

    NEXUS_CORE_INFO("Found " + std::to_string(transformCount) + " entities with Transform components");

    // Test entity management
    auto tempEntity = registry.CreateEntity();
    tempEntity.AddComponent<Nexus::Name>("Temporary");
    registry.DestroyEntity(tempEntity);

    NEXUS_CORE_INFO("ECS system test completed successfully!");
}

int main()
{
    NEXUS_CORE_INFO("Starting NexusEngine with ECS System Test");

    // Debug: Check if we're about to call the ECS test
    NEXUS_CORE_INFO("About to call TestECSSystem()...");

    // Test the ECS system
    TestECSSystem();

    // Debug: Check if ECS test completed
    NEXUS_CORE_INFO("TestECSSystem() call completed");

    // Create window for basic testing
    Nexus::Window window;

    NEXUS_CORE_INFO("Window created successfully - Press ESC or close window to exit");

    // Simple main loop
    while (!window.ShouldClose())
    {
        window.Update();

        // Clear screen to a simple color
        glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        window.SwapBuffers();
    }

    NEXUS_CORE_INFO("NexusEngine shutting down");
    return 0;
}