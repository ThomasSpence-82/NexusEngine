#include "Scene/ECS/Systems/RenderSystem.h"
#include "Renderer/Camera.h"
#include "Renderer/Shader.h"
#include "Renderer/Texture.h"
#include "Core/Logger.h"
#include <windows.h>
#include <GL/gl.h>
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
        m_CheckerTexture = Texture::Create("checkerboard.png");

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

        // Use the camera's matrices
        glMatrixMode(GL_PROJECTION);
        Matrix4 projMatrix = camera.GetProjectionMatrix();
        glLoadMatrixf(projMatrix.Data());

        glMatrixMode(GL_MODELVIEW);
        Matrix4 viewMatrix = camera.GetViewMatrix();
        glLoadMatrixf(viewMatrix.Data());

        // Render all ECS entities with Transform and MeshRenderer
        auto transformView = registry.GetView<Transform>();
        int entitiesRendered = 0;

        for (auto [entity, transform] : transformView)
        {
            if (entity.HasComponent<MeshRenderer>())
            {
                glPushMatrix();

                // Apply ECS transform
                Vector3 pos = transform.position;
                Vector3 scale = transform.scale;

                glTranslatef(pos.x, pos.y, pos.z);

                // Convert quaternion rotation to euler angles
                Vector3 euler = transform.rotation.ToEulerAngles();
                glRotatef(euler.z * 180.0f / 3.14159f, 0, 0, 1);
                glRotatef(euler.y * 180.0f / 3.14159f, 0, 1, 0);
                glRotatef(euler.x * 180.0f / 3.14159f, 1, 0, 0);

                glScalef(scale.x, scale.y, scale.z);

                // Draw single orange cube
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