#pragma once

#include <string>

namespace Nexus
{
    class Texture
    {
    public:
        Texture(const std::string& path);
        ~Texture();

        void Bind(unsigned int slot = 0) const;
        void Unbind() const;

        unsigned int GetWidth() const { return m_Width; }
        unsigned int GetHeight() const { return m_Height; }
        unsigned int GetRendererID() const { return m_RendererID; }

        static Texture* Create(const std::string& path);

    private:
        unsigned int m_RendererID;
        std::string m_FilePath;
        unsigned char* m_LocalBuffer;
        int m_Width, m_Height, m_BPP; // Bits per pixel
    };
}