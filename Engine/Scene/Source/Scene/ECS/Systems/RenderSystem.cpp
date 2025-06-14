#include "Scene/ECS/Systems/RenderSystem.h"
#include "Scene/ECS/Registry.h"
#include "Scene/ECS/Components/Transform.h"
#include "Scene/ECS/Components/MeshRenderer.h"
#include "Renderer/Camera.h"
#include "Renderer/Shader.h"
#include "Renderer/Texture.h"
#include "Core/Logger.h"

// OpenGL includes
#include <windows.h>
#include <GL/gl.h>

// OpenGL constants that might be missing
#ifndef GL_DEPTH_TEST
#define GL_DEPTH_TEST                     0x0B71
#endif
#ifndef GL_CULL_FACE
#define GL_CULL_FACE                      0x0B44
#endif
#ifndef GL_BACK
#define GL_BACK                           0x0405
#endif
#ifndef GL_TEXTURE_2D
#define GL_TEXTURE_2D                     0x0DE1
#endif
#ifndef GL_COLOR_BUFFER_BIT
#define GL_COLOR_BUFFER_BIT               0x00004000
#endif
#ifndef GL_DEPTH_BUFFER_BIT
#define GL_DEPTH_BUFFER_BIT               0x00000100
#endif
#ifndef GL_QUADS
#define GL_QUADS                          0x0007
#endif

#include <cmath>

namespace Nexus
{
    RenderSystem::RenderSystem()
    {
    }

    RenderSystem::~RenderSystem()
    {
        Shutdown();
    }

    void RenderSystem::Initialize()
    {
        if (m_Initialized)
            return;

        NEXUS_CORE_INFO("Initializing RenderSystem...");

        // Simple OpenGL setup for compatibility mode
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        // Enable texturing
        glEnable(GL_TEXTURE_2D);

        // Create the checkered texture using your existing Texture class
        // The filename doesn't matter since your Texture class generates the pattern
        m_CheckerTexture = Texture::Create("checkerboard.png");

        if (m_CheckerTexture)
        {
            NEXUS_CORE_INFO("Checker texture created successfully");
        }
        else
        {
            NEXUS_CORE_ERROR("Failed to create checker texture!");
        }

        m_Initialized = true;
        NEXUS_CORE_INFO("RenderSystem initialized successfully");
    }

    void RenderSystem::LoadOpenGLFunctions()
    {
        // Not needed for compatibility mode
    }

    void RenderSystem::CreateCubeMesh()
    {
        // Not needed for immediate mode
    }

    void RenderSystem::CreateBasicShader()
    {
        // Not needed for fixed-function pipeline
        m_BasicShader = nullptr;
    }

    void RenderSystem::Render(Registry& registry, const Camera& camera)
    {
        if (!m_Initialized)
        {
            NEXUS_CORE_ERROR("RenderSystem not initialized!");
            return;
        }

        // Clear the screen
        glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Enable proper 3D rendering
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);

        // FIXED: Use proper aspect ratio for your 1280x720 window
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        // Calculate proper aspect ratio
        float aspectRatio = 1280.0f / 720.0f; // 16:9 aspect ratio
        float fov = 45.0f; // Field of view in degrees
        float nearPlane = 0.1f;
        float farPlane = 100.0f;

        // Convert FOV to frustum bounds
        float top = nearPlane * tanf(fov * 3.14159f / 360.0f); // Half FOV in radians
        float bottom = -top;
        float right = top * aspectRatio;
        float left = -right;

        glFrustum(left, right, bottom, top, nearPlane, farPlane);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        // Simple camera setup - look at cube from distance
        glTranslatef(0.0f, 0.0f, -5.0f);

        // Render all ECS entities with Transform and MeshRenderer
        auto transformView = registry.GetView<Transform>();
        int entitiesRendered = 0;

        for (auto [entity, transform] : transformView)
        {
            if (entity.HasComponent<MeshRenderer>())
            {
                glPushMatrix();

                // Apply ECS transform - simplified for now
                Vector3 pos = transform.position;
                Vector3 scale = transform.scale;

                glTranslatef(pos.x, pos.y, pos.z);
                glScalef(scale.x, scale.y, scale.z);

                // Add simple rotation using static time-based rotation
                static float rotationAngle = 0.0f;
                rotationAngle += 1.0f; // Increment each frame
                glRotatef(rotationAngle, 0.0f, 1.0f, 0.0f); // Rotate around Y axis

                // Draw single checkered cube
                DrawCheckeredCube();

                glPopMatrix();
                entitiesRendered++;
            }
        }

        // Log occasionally with minimal info
        static int frameCount = 0;
        if (frameCount % 300 == 0) // Every 5 seconds instead of every second
        {
            NEXUS_CORE_INFO("Rendered " + std::to_string(entitiesRendered) + " entities");
        }
        frameCount++;
    }

    void RenderSystem::DrawCheckeredCube()
    {
        // Bind your actual checkered texture
        if (m_CheckerTexture)
        {
            m_CheckerTexture->Bind(0);
        }

        // Set white color so texture shows properly
        glColor3f(1.0f, 1.0f, 1.0f);

        glBegin(GL_QUADS);

        // Front face
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.5f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, -0.5f, 0.5f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.5f, 0.5f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, 0.5f);

        // Back face
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5f, 0.5f, -0.5f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5f, 0.5f, -0.5f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5f, -0.5f, -0.5f);

        // Top face
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, -0.5f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, 0.5f, 0.5f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, 0.5f, 0.5f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.5f, -0.5f);

        // Bottom face
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5f, -0.5f, -0.5f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5f, -0.5f, 0.5f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.5f);

        // Right face
        glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, -0.5f, -0.5f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.5f, -0.5f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5f, 0.5f, 0.5f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5f, -0.5f, 0.5f);

        // Left face
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.5f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5f, 0.5f, 0.5f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, -0.5f);

        glEnd();

        // Unbind texture
        if (m_CheckerTexture)
        {
            m_CheckerTexture->Unbind();
        }
    }

    void RenderSystem::Shutdown()
    {
        // Clean up texture
        if (m_CheckerTexture)
        {
            delete m_CheckerTexture;
            m_CheckerTexture = nullptr;
        }

        m_Initialized = false;
        NEXUS_CORE_INFO("RenderSystem shut down");
    }
}