#pragma once
#include "Math/Vector3.h"

namespace Nexus
{
    // Basic light component
    class Light
    {
    public:
        // Light types
        enum class LightType
        {
            Directional,    // Sun-like light (infinite distance)
            Point,          // Light bulb (radiates in all directions)
            Spot           // Flashlight (cone of light)
        };

        // Light properties
        LightType type = LightType::Directional;
        Vector3 color = Vector3::One;       // RGB color (0-1 range)
        float intensity = 1.0f;             // Light intensity multiplier

        // Point/Spot light properties
        float range = 10.0f;                // Maximum light distance

        // Spot light properties
        float innerConeAngle = 30.0f;       // Inner cone angle in degrees
        float outerConeAngle = 45.0f;       // Outer cone angle in degrees

        // Flags
        bool isActive = true;               // Is light currently active?
        bool castShadows = true;            // Does this light cast shadows?

        // Constructors
        Light() = default;
        Light(LightType lightType, const Vector3& lightColor = Vector3::One, float lightIntensity = 1.0f)
            : type(lightType), color(lightColor), intensity(lightIntensity) {
        }

        // Factory methods for common light types
        static Light CreateDirectional(const Vector3& lightColor = Vector3::One, float lightIntensity = 1.0f)
        {
            return Light(LightType::Directional, lightColor, lightIntensity);
        }

        static Light CreatePoint(float lightRange = 10.0f, const Vector3& lightColor = Vector3::One, float lightIntensity = 1.0f)
        {
            Light light(LightType::Point, lightColor, lightIntensity);
            light.range = lightRange;
            return light;
        }

        static Light CreateSpot(float lightRange = 10.0f, float innerAngle = 30.0f, float outerAngle = 45.0f,
            const Vector3& lightColor = Vector3::One, float lightIntensity = 1.0f)
        {
            Light light(LightType::Spot, lightColor, lightIntensity);
            light.range = lightRange;
            light.innerConeAngle = innerAngle;
            light.outerConeAngle = outerAngle;
            return light;
        }

        std::string ToString() const
        {
            const char* typeNames[] = { "Directional", "Point", "Spot" };
            return "Light(" + std::string(typeNames[static_cast<int>(type)]) +
                ", Color: " + color.ToString() +
                ", Intensity: " + std::to_string(intensity) + ")";
        }
    };
}
