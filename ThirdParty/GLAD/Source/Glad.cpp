#include "glad.h"
#include <iostream>

// Our OpenGL function implementations
void APIENTRY nexus_glClear(GLbitfield mask)
{
    // Get the real function from opengl32.dll
    static auto real_glClear = (void(APIENTRY*)(GLbitfield))GetProcAddress(GetModuleHandleA("opengl32.dll"), "glClear");
    if (real_glClear)
        real_glClear(mask);
}

void APIENTRY nexus_glClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    static auto real_glClearColor = (void(APIENTRY*)(GLfloat, GLfloat, GLfloat, GLfloat))GetProcAddress(GetModuleHandleA("opengl32.dll"), "glClearColor");
    if (real_glClearColor)
        real_glClearColor(red, green, blue, alpha);
}

const GLubyte* APIENTRY nexus_glGetString(GLenum name)
{
    static auto real_glGetString = (const GLubyte * (APIENTRY*)(GLenum))GetProcAddress(GetModuleHandleA("opengl32.dll"), "glGetString");
    if (real_glGetString)
        return real_glGetString(name);
    return nullptr;
}

int gladLoadGL(void)
{
    // Check if we can access opengl32.dll
    HMODULE opengl32 = GetModuleHandleA("opengl32.dll");
    if (!opengl32)
    {
        std::cout << "OpenGL not available" << std::endl;
        return 0;
    }

    std::cout << "GLAD: OpenGL functions loaded successfully" << std::endl;
    return 1;
}