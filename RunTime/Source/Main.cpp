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
#include "Input/InputManager.h"  // NEW: For camera controls
#include <windows.h>
#include <GL/gl.h>

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

    // Initialize InputManager
    Nexus::InputManager::Initialize();

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

        // Handle camera movement with WASD keys
        float cameraSpeed = 0.2f; // REDUCED speed
        static int debugFrame = 0; // Declare debugFrame here

        if (Nexus::InputManager::IsKeyDown(Nexus::KeyCode::W))
        {
            renderCamera.MoveForward(cameraSpeed);
        }
        if (Nexus::InputManager::IsKeyDown(Nexus::KeyCode::S))
        {
            renderCamera.MoveForward(-cameraSpeed);
        }
        if (Nexus::InputManager::IsKeyDown(Nexus::KeyCode::A))
        {
            renderCamera.MoveRight(-cameraSpeed);
        }
        if (Nexus::InputManager::IsKeyDown(Nexus::KeyCode::D))
        {
            renderCamera.MoveRight(cameraSpeed);
        }
        if (Nexus::InputManager::IsKeyDown(Nexus::KeyCode::Q))
        {
            renderCamera.MoveUp(-cameraSpeed);
        }
        if (Nexus::InputManager::IsKeyDown(Nexus::KeyCode::E))
        {
            renderCamera.MoveUp(cameraSpeed);
        }

        debugFrame++;

        // Handle camera rotation with arrow keys
        float rotationSpeed = 2.0f; // Degrees per frame

        if (Nexus::InputManager::IsKeyDown(Nexus::KeyCode::Left))
        {
            renderCamera.RotateYaw(-rotationSpeed * 3.14159f / 180.0f);
        }
        if (Nexus::InputManager::IsKeyDown(Nexus::KeyCode::Right))
        {
            renderCamera.RotateYaw(rotationSpeed * 3.14159f / 180.0f);
        }
        if (Nexus::InputManager::IsKeyDown(Nexus::KeyCode::Up))
        {
            renderCamera.RotatePitch(-rotationSpeed * 3.14159f / 180.0f);
        }
        if (Nexus::InputManager::IsKeyDown(Nexus::KeyCode::Down))
        {
            renderCamera.RotatePitch(rotationSpeed * 3.14159f / 180.0f);
        }

        // Reset camera position if needed  
        if (Nexus::InputManager::IsKeyPressed(Nexus::KeyCode::R))
        {
            renderCamera.SetPosition(Nexus::Vector3(0.0f, 0.0f, 5.0f));
            renderCamera.SetRotation(Nexus::Vector3(0.0f, 0.0f, 0.0f));
        }

        // Space for moving up quickly
        if (Nexus::InputManager::IsKeyDown(Nexus::KeyCode::Space))
        {
            renderCamera.MoveUp(cameraSpeed * 2.0f);
        }

        // Rotate the cube for visual interest
        cubeTransform.Rotate(Nexus::Quaternion::FromEulerAngles(Nexus::Vector3(0.01f, 0.02f, 0.005f)));

        // Render all ECS entities (RenderSystem will handle clearing)
        renderSystem.Render(renderRegistry, renderCamera);

        window.SwapBuffers();
    }

    NEXUS_CORE_INFO("NexusEngine shutting down");

    // Cleanup
    Nexus::InputManager::Shutdown();

    return 0;
}