#pragma once
#include <string>

namespace Nexus
{
    // Name component for entity identification
    class Name
    {
    public:
        std::string name;

        Name() : name("Entity") {}
        Name(const std::string& entityName) : name(entityName) {}

        std::string ToString() const
        {
            return "Name(" + name + ")";
        }
    };

    // Tag component for grouping entities
    class Tag
    {
    public:
        std::string tag;

        Tag() : tag("Untagged") {}
        Tag(const std::string& entityTag) : tag(entityTag) {}

        bool HasTag(const std::string& checkTag) const
        {
            return tag == checkTag;
        }

        std::string ToString() const
        {
            return "Tag(" + tag + ")";
        }
    };
}