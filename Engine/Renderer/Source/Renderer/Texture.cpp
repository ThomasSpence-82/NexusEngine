#include "Renderer/Texture.h"
#include "Core/Logger.h"
#include <windows.h>
#include <GL/gl.h>

// We'll use a simple image loading approach for now
// For a real engine, you'd use stb_image or similar

// OpenGL texture constants
#define GL_TEXTURE_2D                     0x0DE1
#define GL_TEXTURE_MAG_FILTER             0x2800
#define GL_TEXTURE_MIN_FILTER             0x2801
#define GL_TEXTURE_WRAP_S                 0x2802
#define GL_TEXTURE_WRAP_T                 0x2803
#define GL_REPEAT                         0x2901
#define GL_LINEAR                         0x2601
#define GL_RGBA                           0x1908
#define GL_RGB                            0x1907
#define GL_UNSIGNED_BYTE                  0x1401

// OpenGL function pointers for textures
typedef void (APIENTRY *PFNGLGENTEXTURESPROC)(int n, unsigned int* textures);
typedef void (APIENTRY *PFNGLBINDTEXTUREPROC)(unsigned int target, unsigned int texture);
typedef void (APIENTRY *PFNGLTEXIMAGE2DPROC)(unsigned int target, int level, int internalformat, int width, int height, int border, unsigned int format, unsigned int type, const void* pixels);
typedef void (APIENTRY *PFNGLTEXPARAMETERIPROC)(unsigned int target, unsigned int pname, int param);
typedef void (APIENTRY *PFNGLACTIVETEXTUREPROC)(unsigned int texture);
typedef void (APIENTRY *PFNGLDELETETEXTURESPROC)(int n, const unsigned int* textures);

namespace Nexus
{
    static PFNGLGENTEXTURESPROC glGenTextures = nullptr;
    static PFNGLBINDTEXTUREPROC glBindTexture = nullptr;
    static PFNGLTEXIMAGE2DPROC glTexImage2D = nullptr;
    static PFNGLTEXPARAMETERIPROC glTexParameteri = nullptr;
    static PFNGLACTIVETEXTUREPROC glActiveTexture = nullptr;
    static PFNGLDELETETEXTURESPROC glDeleteTextures = nullptr;
    
    static bool s_TextureFunctionsLoaded = false;

    static void LoadTextureFunctions()
    {
        if (s_TextureFunctionsLoaded) return;

        // Try to load from wglGetProcAddress first, fall back to GetProcAddress
        glGenTextures = (PFNGLGENTEXTURESPROC)wglGetProcAddress("glGenTextures");
        if (!glGenTextures)
            glGenTextures = (PFNGLGENTEXTURESPROC)GetProcAddress(GetModuleHandleA("opengl32.dll"), "glGenTextures");

        glBindTexture = (PFNGLBINDTEXTUREPROC)wglGetProcAddress("glBindTexture");
        if (!glBindTexture)
            glBindTexture = (PFNGLBINDTEXTUREPROC)GetProcAddress(GetModuleHandleA("opengl32.dll"), "glBindTexture");

        glTexImage2D = (PFNGLTEXIMAGE2DPROC)wglGetProcAddress("glTexImage2D");
        if (!glTexImage2D)
            glTexImage2D = (PFNGLTEXIMAGE2DPROC)GetProcAddress(GetModuleHandleA("opengl32.dll"), "glTexImage2D");

        glTexParameteri = (PFNGLTEXPARAMETERIPROC)wglGetProcAddress("glTexParameteri");
        if (!glTexParameteri)
            glTexParameteri = (PFNGLTEXPARAMETERIPROC)GetProcAddress(GetModuleHandleA("opengl32.dll"), "glTexParameteri");

        glActiveTexture = (PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture");
        glDeleteTextures = (PFNGLDELETETEXTURESPROC)wglGetProcAddress("glDeleteTextures");
        if (!glDeleteTextures)
            glDeleteTextures = (PFNGLDELETETEXTURESPROC)GetProcAddress(GetModuleHandleA("opengl32.dll"), "glDeleteTextures");

        s_TextureFunctionsLoaded = true;
        NEXUS_CORE_INFO("Texture functions loaded");
    }

    // Simple procedural texture generation for now (until we add image loading)
    static unsigned char* GenerateTestTexture(int& width, int& height, int& channels)
    {
        width = 256;
        height = 256;
        channels = 3;
        
        unsigned char* data = new unsigned char[width * height * channels];
        
        // Generate a simple checkerboard pattern
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                int index = (y * width + x) * channels;
                
                // Create checkerboard pattern
                bool isWhite = ((x / 32) + (y / 32)) % 2 == 0;
                
                if (isWhite)
                {
                    data[index + 0] = 255; // R
                    data[index + 1] = 255; // G  
                    data[index + 2] = 255; // B
                }
                else
                {
                    data[index + 0] = 64;  // R
                    data[index + 1] = 64;  // G
                    data[index + 2] = 64;  // B
                }
            }
        }
        
        return data;
    }

    Texture::Texture(const std::string& path)
        : m_RendererID(0), m_FilePath(path), m_LocalBuffer(nullptr), m_Width(0), m_Height(0), m_BPP(0)
    {
        LoadTextureFunctions();

        // For now, generate a test texture (we'll add real image loading later)
        m_LocalBuffer = GenerateTestTexture(m_Width, m_Height, m_BPP);

        glGenTextures(1, &m_RendererID);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_LocalBuffer);

        glBindTexture(GL_TEXTURE_2D, 0);

        NEXUS_CORE_INFO("Texture created: " + std::to_string(m_Width) + "x" + std::to_string(m_Height) + " (ID: " + std::to_string(m_RendererID) + ")");
    }

    Texture::~Texture()
    {
        if (glDeleteTextures && m_RendererID)
            glDeleteTextures(1, &m_RendererID);
        
        if (m_LocalBuffer)
            delete[] m_LocalBuffer;
    }

    void Texture::Bind(unsigned int slot) const
    {
        if (glActiveTexture)
            glActiveTexture(0x84C0 + slot); // GL_TEXTURE0 + slot
        
        if (glBindTexture)
            glBindTexture(GL_TEXTURE_2D, m_RendererID);
    }

    void Texture::Unbind() const
    {
        if (glBindTexture)
            glBindTexture(GL_TEXTURE_2D, 0);
    }

    Texture* Texture::Create(const std::string& path)
    {
        return new Texture(path);
    }
}