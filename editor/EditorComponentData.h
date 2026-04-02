#pragma once

#include <map>
#include <string>

namespace dae
{
    struct ComponentInstance
    {
        std::string componentType;
        std::string componentName;
        std::map<std::string, std::string> properties;
    };
}
