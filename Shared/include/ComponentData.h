#ifndef _COMPONENTDATA_H_
#define _COMPONENTDATA_H_

#include <map>
#include <string>

namespace dae
{
    /// <summary>
    /// Shared component instance data structure used for serialization/deserialization
    /// </summary>
    struct ComponentInstance
    {
        std::string componentType;
        unsigned int componentTypeHash = 0;
        std::string componentName;
        std::map<std::string, std::string> properties;
    };
}

#endif
