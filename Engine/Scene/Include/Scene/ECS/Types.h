#pragma once
#include <cstdint>
#include <typeinfo>
#include <typeindex>

namespace Nexus
{
    // Entity ID type - simple uint32 with special values
    using EntityID = uint32_t;

    // Component type identifier
    using ComponentTypeID = std::type_index;

    // Constants
    constexpr EntityID NULL_ENTITY = 0;
    constexpr EntityID MAX_ENTITIES = 1000000; // 1 million entities max
    constexpr size_t MAX_COMPONENTS = 128;      // 128 different component types max

    // Helper for generating component type IDs
    template<typename T>
    ComponentTypeID GetComponentTypeID()
    {
        return std::type_index(typeid(T));
    }

    // Component storage index type
    using ComponentIndex = size_t;
    constexpr ComponentIndex INVALID_COMPONENT_INDEX = SIZE_MAX;
}
