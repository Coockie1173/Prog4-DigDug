#ifndef COMPONENT_PARSER_H
#define COMPONENT_PARSER_H

#include <string>
#include <vector>

class ComponentParser
{
public:
    bool ParseComponentDirectory(const std::string& componentDir, const std::string& gameObjectPath, const std::string& outputDir);

private:
    struct Property
    {
        std::string type;
        std::string name;
        std::string displayName;
        std::string tooltip;
    };

    struct ComponentInfo
    {
        std::string className;
        std::string filename;
        std::vector<Property> exposedProperties;
        std::vector<Property> constructorParams;
        std::vector<Property> properties;
        std::string bareBonesClassName;
    };

    bool ParseComponentFile(const std::string& filepath, ComponentInfo& info);
    bool GenerateBarebonesComponent(const ComponentInfo& info, const std::string& outputPath);
    bool GenerateBaseComponentBarebones(const std::string& outputDir);
    bool GenerateRegisterMaster(const std::vector<ComponentInfo>& components, const std::string& outputDir);
    bool GenerateBarebonesGameObject(const std::string& gameObjectPath, const std::string& outputDir);

    void WriteBarebonesHeader(std::ofstream& file, const std::string& guardName, const std::string& baseClass, 
                              const std::vector<Property>& properties, bool isBaseClass = false);

    bool ExtractPropertyFromLine(const std::string& line, Property& prop);
};

#endif // COMPONENT_PARSER_H
