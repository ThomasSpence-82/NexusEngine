#include "Core/Logger.h"
#include "Core/Window.h"
#include "Renderer/Camera.h"
#include "Scene/ECS/Registry.h"
#include "Scene/ECS/Components/Transform.h"
#include "Scene/ECS/Components/Name.h"
#include "Scene/ECS/Components/CameraComponent.h"
#include "Scene/ECS/Components/Light.h"
#include "Scene/ECS/Components/MeshRenderer.h"
#include "Scene/ECS/Systems/RenderSystem.h"
#include "Input/InputManager.h"
#include <windows.h>
#include <GL/gl.h>

void ComprehensiveECSTest()
{
    NEXUS_CORE_INFO("=== Comprehensive ECS System Test ===");

    Nexus::Registry registry;

    // Test 1: Create multiple entities with different component combinations
    auto cubeEntity = registry.CreateEntity();
    auto lightEntity = registry.CreateEntity();
    auto cameraEntity = registry.CreateEntity();
    auto orphanEntity = registry.CreateEntity(); // Entity with no components

    NEXUS_CORE_INFO("Created 4 entities");

    // Test 2: Add various component combinations
    cubeEntity.AddComponent<Nexus::Transform>(Nexus::Vector3(1, 0, 0));
    cubeEntity.AddComponent<Nexus::MeshRenderer>("cube.obj", "default.mat");
    cubeEntity.AddComponent<Nexus::Name>("Test Cube");

    lightEntity.AddComponent<Nexus::Transform>(Nexus::Vector3(2, 2, 2));
    lightEntity.AddComponent<Nexus::Light>(Nexus::Light::CreateDirectional());
    lightEntity.AddComponent<Nexus::Name>("Test Light");
    // Note: lightEntity has NO MeshRenderer

    cameraEntity.AddComponent<Nexus::Transform>(Nexus::Vector3(0, 0, 5));
    cameraEntity.AddComponent<Nexus::CameraComponent>();
    cameraEntity.AddComponent<Nexus::Name>("Test Camera");
    // Note: cameraEntity has NO MeshRenderer

    // orphanEntity has NO components

    NEXUS_CORE_INFO("Added components to entities");

    // Test 3: Component modification
    auto& cubeTransform = cubeEntity.GetComponent<Nexus::Transform>();
    cubeTransform.SetPosition(Nexus::Vector3(0, 1, 0));
    NEXUS_CORE_INFO("Modified cube position");

    // Test 4: Single component queries
    auto transformView = registry.GetView<Nexus::Transform>();
    int transformCount = 0;
    for (auto [entity, transform] : transformView)
    {
        transformCount++;
        NEXUS_CORE_INFO("Entity with Transform at: " + transform.position.ToString());
    }
    NEXUS_CORE_INFO("Found " + std::to_string(transformCount) + " entities with Transform (should be 3)");

    // Test 5: Check component existence
    NEXUS_CORE_INFO("Component existence tests:");
    NEXUS_CORE_INFO("Cube has MeshRenderer: " + std::string(cubeEntity.HasComponent<Nexus::MeshRenderer>() ? "YES" : "NO"));
    NEXUS_CORE_INFO("Light has MeshRenderer: " + std::string(lightEntity.HasComponent<Nexus::MeshRenderer>() ? "YES" : "NO"));
    NEXUS_CORE_INFO("Camera has MeshRenderer: " + std::string(cameraEntity.HasComponent<Nexus::MeshRenderer>() ? "YES" : "NO"));
    NEXUS_CORE_INFO("Orphan has Transform: " + std::string(orphanEntity.HasComponent<Nexus::Transform>() ? "YES" : "NO"));

    // Test 6: Multi-component logic (manual for now)
    int renderableEntities = 0;
    for (auto [entity, transform] : transformView)
    {
        if (entity.HasComponent<Nexus::MeshRenderer>())
        {
            renderableEntities++;
            auto& meshRenderer = entity.GetComponent<Nexus::MeshRenderer>();
            NEXUS_CORE_INFO("Renderable entity: " + meshRenderer.ToString());
        }
    }
    NEXUS_CORE_INFO("Found " + std::to_string(renderableEntities) + " renderable entities (should be 1)");

    // Test 7: Component removal
    cubeEntity.RemoveComponent<Nexus::Name>();
    NEXUS_CORE_INFO("Removed Name component from cube");
    NEXUS_CORE_INFO("Cube still has Name: " + std::string(cubeEntity.HasComponent<Nexus::Name>() ? "YES" : "NO"));

    // Test 8: Entity destruction and reuse
    auto oldOrphanID = orphanEntity.GetID();
    registry.DestroyEntity(orphanEntity);
    NEXUS_CORE_INFO("Destroyed orphan entity with ID: " + std::to_string(oldOrphanID));

    // Create new entity - should reuse the ID
    auto newEntity = registry.CreateEntity();
    auto newEntityID = newEntity.GetID();
    NEXUS_CORE_INFO("Created new entity with ID: " + std::to_string(newEntityID));
    NEXUS_CORE_INFO("ID was reused: " + std::string(newEntityID == oldOrphanID ? "YES" : "NO"));

    // Test 9: Verify destroyed entity doesn't appear in queries
    int finalTransformCount = 0;
    for (auto [entity, transform] : registry.GetView<Nexus::Transform>())
    {
        finalTransformCount++;
    }
    NEXUS_CORE_INFO("Transform entities after destruction: " + std::to_string(finalTransformCount) + " (should still be 3)");

    NEXUS_CORE_INFO("=== ECS Test Complete ===");
}

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
    auto& cameraComp = camera.AddComponent<Nexus::CameraComponent>();
    cameraComp.isPrimary = true;

    // Add components to cube
    cube.AddComponent<Nexus::Name>("Test Cube");
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

    // Initialize InputManager
    Nexus::InputManager::Initialize();

    // Run comprehensive ECS test first
    ComprehensiveECSTest();

    // Test the ECS system
    TestECSSystem();

    // Create window for rendering
    Nexus::Window window;

    // Create camera and ECS registry for rendering
    Nexus::Camera renderCamera(45.0f, 1280.0f / 720.0f, 0.1f, 100.0f);

    // Simple position directly in front of cube - guaranteed to work
    renderCamera.SetPosition(Nexus::Vector3(0.0f, 0.0f, 5.0f)); // Straight back from cube
    renderCamera.SetRotation(Nexus::Vector3(0.0f, 0.0f, 0.0f)); // Looking straight forward

    Nexus::Registry renderRegistry;
    Nexus::RenderSystem renderSystem;

    // Initialize the render system
    renderSystem.Initialize();

    // Create a cube entity for rendering
    auto cube = renderRegistry.CreateEntity();
    cube.AddComponent<Nexus::Name>("Rendered Cube");
    auto& cubeTransform = cube.AddComponent<Nexus::Transform>();
    cubeTransform.SetPosition(Nexus::Vector3(0, 0, 0));
    cube.AddComponent<Nexus::MeshRenderer>("cube.obj", "default.mat");

    NEXUS_CORE_INFO("Window created successfully - Your cube should be visible!");
    NEXUS_CORE_INFO("Controls: ESC or close window to exit");

    // Main rendering loop
    while (!window.ShouldClose())
    {
        window.Update();

        // Simple time-based rotation for visual interest
        static float totalTime = 0.0f;
        totalTime += 0.016f; // Approximate 60 FPS

        // Apply rotation to the cube transform
        cubeTransform.SetEulerAngles(Nexus::Vector3(totalTime * 0.5f, totalTime, totalTime * 0.3f));

        // Render all ECS entities (RenderSystem will handle clearing)
        renderSystem.Render(renderRegistry, renderCamera);

        window.SwapBuffers();
    }

    NEXUS_CORE_INFO("NexusEngine shutting down");

    // Cleanup
    Nexus::InputManager::Shutdown();

    return 0;
}