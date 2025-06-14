#pragma once

#include "Scene/ECS/Registry.h"
#include "Scene/ECS/Components/Transform.h"
#include "Scene/ECS/Components/MeshRenderer.h"

namespace Nexus
{
    class Camera;
    class Shader;
    class Texture;

    class RenderSystem
    {
    public:
        RenderSystem();
        ~RenderSystem();

        void Initialize();
        void Render(Registry& registry, const Camera& camera);
        void Shutdown();

    private:
        void LoadOpenGLFunctions();
        void CreateCubeMesh();
        void CreateBasicShader();
        void DrawCheckeredCube();

        bool m_Initialized = false;

        // OpenGL resources
        unsigned int m_CubeVAO = 0;
        unsigned int m_CubeVBO = 0;
        unsigned int m_CubeEBO = 0;

        // Shader
        Shader* m_BasicShader = nullptr;

        // Texture
        Texture* m_CheckerTexture = nullptr;
    };
}