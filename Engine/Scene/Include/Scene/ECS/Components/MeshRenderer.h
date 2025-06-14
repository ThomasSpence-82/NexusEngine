#pragma once
#include <string>
#include <cstdint>

namespace Nexus
{
    // Simple mesh renderer component (references to assets)
    class MeshRenderer
    {
    public:
        // Asset references (using IDs for now, will integrate with asset system later)
        uint32_t meshID = 0;        // Reference to mesh asset
        uint32_t materialID = 0;    // Reference to material asset

        // Rendering properties
        bool castShadows = true;
        bool receiveShadows = true;
        bool visible = true;

        // Constructors
        MeshRenderer() = default;
        MeshRenderer(uint32_t mesh, uint32_t material)
            : meshID(mesh), materialID(material) {
        }

        // For now, using simple file paths (will be replaced with asset system)
        std::string meshPath;       // Temporary: path to mesh file
        std::string materialPath;   // Temporary: path to material file

        MeshRenderer(const std::string& mesh, const std::string& material)
            : meshPath(mesh), materialPath(material) {
        }

        bool IsValid() const
        {
            return meshID != 0 || !meshPath.empty();
        }

        std::string ToString() const
        {
            if (!meshPath.empty())
            {
                return "MeshRenderer(mesh: " + meshPath + ", material: " + materialPath + ")";
            }
            return "MeshRenderer(meshID: " + std::to_string(meshID) +
                ", materialID: " + std::to_string(materialID) + ")";
        }
    };
}
