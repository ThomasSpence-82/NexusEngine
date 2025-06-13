#pragma once
#include "Math/Matrix4.h"
#include "Math/Vector3.h"

namespace Nexus
{
    class Shader;

    class Gizmo
    {
    public:
        Gizmo();
        ~Gizmo();

        void Render(const Matrix4& viewProjectionMatrix, const Vector3& position = Vector3::Zero);
        void SetScreenSpace(bool screenSpace);
        bool IsScreenSpace() const { return m_ScreenSpace; }

        static Gizmo* Create();

    private:
        void Initialize();

    private:
        unsigned int m_VAO, m_VBO;
        Shader* m_Shader;
        bool m_ScreenSpace; // true = screen corner, false = world space
    };
}