#include "ComponentParser.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <regex>
#include <unordered_set>

namespace fs = std::filesystem;

bool ComponentParser::ParseComponentDirectory(const std::string& componentDir, const std::string& gameObjectPath, const std::string& outputDir)
{
    if (!fs::exists(componentDir))
    {
        std::cerr << "Component directory not found: " << componentDir << std::endl;
        return false;
    }

    fs::create_directories(outputDir);
    std::vector<ComponentInfo> components;
    std::unordered_set<std::string> seenClassNames;

    for (const auto& entry : fs::recursive_directory_iterator(componentDir))
    {
        if (entry.path().extension() == ".h" && entry.path().filename().string().find("Component") != std::string::npos)
        {
            ComponentInfo info;
            if (ParseComponentFile(entry.path().string(), info))
            {
                if (seenClassNames.find(info.className) == seenClassNames.end())
                {
                    seenClassNames.insert(info.className);
                    std::string outputFilename = (fs::path(outputDir) / (info.className + "_Barebones.h")).string();
                    if (GenerateBarebonesComponent(info, outputFilename))
                    {
                        components.push_back(info);
                    }
                }
            }
        }
    }

    // Generate the base Component_Barebones class first
    GenerateBaseComponentBarebones(outputDir);

    // Generate the master registration file
    if (!components.empty())
    {
        GenerateRegisterMaster(components, outputDir);
    }

    GenerateBarebonesGameObject(gameObjectPath, outputDir);

    return true;
}

bool ComponentParser::ParseComponentFile(const std::string& filepath, ComponentInfo& info)
{
    info.filename = fs::path(filepath).filename().string();

    std::ifstream file(filepath);
    if (!file.is_open())
    {
        std::cerr << "Failed to open: " << filepath << std::endl;
        return false;
    }

    std::string line;
    std::string prevLine;
    bool foundClass = false;
    bool skipComponent = false;

    while (std::getline(file, line))
    {
        // Check for NOEXPOSE directive
        if (line.find("// NOEXPOSE") != std::string::npos)
        {
            skipComponent = true;
        }

        // Look for class definition (not forward declarations)
        if (!foundClass && line.find("class ") != std::string::npos && line.find("Component") != std::string::npos)
        {
            // Skip forward declarations (those ending with ;)
            if (line.find(";") != std::string::npos && line.find("{") == std::string::npos && line.find(":") == std::string::npos)
            {
                // This is a forward declaration, skip it
            }
            else
            {
                size_t classPos = line.find("class ");
                if (classPos != std::string::npos)
                {
                    size_t start = classPos + 6;
                    size_t end = line.find_first_of(" \t:;{", start);
                    if (end == std::string::npos)
                        end = line.length();

                    info.className = line.substr(start, end - start);

                    // Trim whitespace
                    info.className.erase(0, info.className.find_first_not_of(" \t"));
                    info.className.erase(info.className.find_last_not_of(" \t") + 1);

                    if (info.className != "final")
                    {
                        foundClass = true;
                    }
                }
            }
        }

        // Look for EXPOSE_TO_EDITOR annotations
        if (prevLine.find("EXPOSE_TO_EDITOR") != std::string::npos && foundClass)
        {
            Property prop;
            if (ExtractPropertyFromLine(line, prop))
            {
                // Extract metadata from annotation if present
                size_t openParen = prevLine.find("(");
                size_t closeParen = prevLine.find(")");
                if (openParen != std::string::npos && closeParen != std::string::npos)
                {
                    std::string metadata = prevLine.substr(openParen + 1, closeParen - openParen - 1);

                    // Extract display name (first string)
                    size_t firstQuote = metadata.find("\"");
                    if (firstQuote != std::string::npos)
                    {
                        size_t secondQuote = metadata.find("\"", firstQuote + 1);
                        if (secondQuote != std::string::npos)
                        {
                            prop.displayName = metadata.substr(firstQuote + 1, secondQuote - firstQuote - 1);
                        }
                    }

                    // Extract tooltip (second string)
                    size_t thirdQuote = metadata.find("\"", firstQuote + 1);
                    if (thirdQuote != std::string::npos)
                    {
                        thirdQuote = metadata.find("\"", thirdQuote + 1);
                        if (thirdQuote != std::string::npos)
                        {
                            size_t fourthQuote = metadata.find("\"", thirdQuote + 1);
                            if (fourthQuote != std::string::npos)
                            {
                                prop.tooltip = metadata.substr(thirdQuote + 1, fourthQuote - thirdQuote - 1);
                            }
                        }
                    }
                }

                // If no display name was provided, use the property name
                if (prop.displayName.empty())
                {
                    prop.displayName = prop.name;
                }

                info.exposedProperties.push_back(prop);
            }
        }

        prevLine = line;
    }

    file.close();
    return !info.className.empty() && !skipComponent;
}

bool ComponentParser::ExtractPropertyFromLine(const std::string& line, Property& prop)
{
    std::string trimmed = line;
    trimmed.erase(0, trimmed.find_first_not_of(" \t"));

    if (trimmed.empty() || trimmed[0] == '/' || trimmed[0] == '*')
        return false;

    std::regex propRegex(R"(^\s*([a-zA-Z_][a-zA-Z0-9_:<>*&,\s]*?)\s+([a-zA-Z_][a-zA-Z0-9_]*)\s*(?:\{[^}]*\})?.*?;)");
    std::smatch match;

    if (std::regex_search(line, match, propRegex))
    {
        prop.type = match[1].str();
        prop.name = match[2].str();

        prop.type.erase(0, prop.type.find_first_not_of(" \t"));
        prop.type.erase(prop.type.find_last_not_of(" \t") + 1);

        if (prop.name.length() > 2 && prop.name[0] == 'm' && prop.name[1] == '_')
        {
            prop.name = prop.name.substr(2);
        }

        return !prop.type.empty() && !prop.name.empty();
    }

    return false;
}

void ComponentParser::WriteBarebonesHeader(std::ofstream& file, const std::string& guardName, 
                                           const std::string& baseClass, const std::vector<Property>& properties, 
                                           bool isBaseClass)
{
    file << "#ifndef " << guardName << "\n";
    file << "#define " << guardName << "\n\n";
    file << "#include <glm/glm.hpp>\n";
    file << "#include <string>\n";

    if (!baseClass.empty() && !isBaseClass)
    {
        file << "#include \"Component_Barebones.h\"\n";
    }
    file << "\n";
    file << "namespace dae\n";
    file << "{\n";
}

bool ComponentParser::GenerateBaseComponentBarebones(const std::string& outputDir)
{
    std::string headerPath = (fs::path(outputDir) / "Component_Barebones.h").string();
    std::ofstream headerFile(headerPath);
    if (!headerFile.is_open())
    {
        std::cerr << "Failed to create Component_Barebones.h" << std::endl;
        return false;
    }

    std::string guardName = "COMPONENT_BAREBONES_H";

    headerFile << "#ifndef " << guardName << "\n";
    headerFile << "#define " << guardName << "\n\n";
    headerFile << "namespace dae\n";
    headerFile << "{\n";
    headerFile << "    class Component_Barebones\n";
    headerFile << "    {\n";
    headerFile << "    public:\n";
    headerFile << "        Component_Barebones() = default;\n";
    headerFile << "        virtual ~Component_Barebones() = default;\n\n";
    headerFile << "        virtual void RegisterComponentStatic() = 0;\n\n";
    headerFile << "    private:\n";
    headerFile << "        Component_Barebones(const Component_Barebones&) = delete;\n";
    headerFile << "        Component_Barebones& operator=(const Component_Barebones&) = delete;\n";
    headerFile << "    };\n";
    headerFile << "}\n\n";
    headerFile << "#endif // " << guardName << "\n";

    headerFile.close();
    return true;
}

bool ComponentParser::GenerateBarebonesComponent(const ComponentInfo& info, const std::string& outputPath)
{
    std::ofstream outFile(outputPath);
    if (!outFile.is_open())
    {
        std::cerr << "Failed to create output file: " << outputPath << std::endl;
        return false;
    }

    std::string guardName = info.className + "_BAREBONES_H";

    outFile << "#ifndef " << guardName << "\n";
    outFile << "#define " << guardName << "\n\n";
    outFile << "#include <glm/glm.hpp>\n";
    outFile << "#include <string>\n";

    // Check if any properties use SDL types
    bool hasSDLColor = false;
    for (const auto& prop : info.exposedProperties)
    {
        if (prop.type.find("SDL_Color") != std::string::npos)
        {
            hasSDLColor = true;
            break;
        }
    }

    if (hasSDLColor)
    {
        outFile << "#include <SDL3_ttf/SDL_ttf.h>\n";
    }

    outFile << "#include \"Component_Barebones.h\"\n\n";
    outFile << "namespace dae\n";
    outFile << "{\n";
    outFile << "    class " << info.className << "_Barebones : public Component_Barebones\n";
    outFile << "    {\n";
    outFile << "    public:\n";
    outFile << "        " << info.className << "_Barebones() = default;\n";
    outFile << "        virtual ~" << info.className << "_Barebones() = default;\n";
    outFile << "\n";

    for (const auto& prop : info.exposedProperties)
    {
        std::string accessorName = prop.name;
        if (!accessorName.empty())
        {
            accessorName[0] = static_cast<char>(std::toupper(accessorName[0]));
        }
        outFile << "        const " << prop.type << "& Get" << accessorName << "() const { return m_" << prop.name << "; }\n";
        outFile << "        void Set" << accessorName << "(const " << prop.type << "& value) { m_" << prop.name << " = value; }\n";
        outFile << "\n";
    }

    outFile << "        void RegisterComponentStatic() override;\n";
    outFile << "\n";
    outFile << "    private:\n";
    outFile << "        " << info.className << "_Barebones(const " << info.className << "_Barebones&) = delete;\n";
    outFile << "        " << info.className << "_Barebones& operator=(const " << info.className << "_Barebones&) = delete;\n";
    outFile << "\n";

    // Generate member variables
    for (const auto& prop : info.exposedProperties)
    {
        outFile << "        " << prop.type << " m_" << prop.name << "{};\n";
    }

    outFile << "    };\n";
    outFile << "}\n\n";
    outFile << "#endif // " << guardName << "\n";

    outFile.close();
    return true;
}

bool ComponentParser::GenerateRegisterMaster(const std::vector<ComponentInfo>& components, const std::string& outputDir)
{
    // Generate header file
    std::string headerPath = (fs::path(outputDir) / "ComponentRegisterMaster.h").string();
    std::ofstream headerFile(headerPath);
    if (!headerFile.is_open())
    {
        std::cerr << "Failed to create ComponentRegisterMaster.h" << std::endl;
        return false;
    }

    headerFile << "#ifndef COMPONENT_REGISTER_MASTER_H\n";
    headerFile << "#define COMPONENT_REGISTER_MASTER_H\n\n";
    headerFile << "#include <string>\n";
    headerFile << "#include <vector>\n";
    headerFile << "#include <functional>\n\n";
    headerFile << "namespace dae\n";
    headerFile << "{\n";
    headerFile << "    struct PropertyMetadata\n";
    headerFile << "    {\n";
    headerFile << "        std::string type;\n";
    headerFile << "        std::string name;\n";
    headerFile << "        std::string displayName;\n";
    headerFile << "        std::string tooltip;\n";
    headerFile << "    };\n\n";
    headerFile << "    struct ComponentMetadata\n";
    headerFile << "    {\n";
    headerFile << "        std::string componentName;\n";
    headerFile << "        std::string bareBonesClassName;\n";
    headerFile << "        std::vector<PropertyMetadata> properties;\n";
    headerFile << "    };\n\n";
    headerFile << "    const std::vector<ComponentMetadata>& GetAllComponentMetadata();\n";
    headerFile << "    void RegisterAllComponents();\n";
    headerFile << "}\n\n";
    headerFile << "#endif // COMPONENT_REGISTER_MASTER_H\n";
    headerFile.close();

    // Generate implementation file
    std::string implPath = (fs::path(outputDir) / "ComponentRegisterMaster.cpp").string();
    std::ofstream implFile(implPath);
    if (!implFile.is_open())
    {
        std::cerr << "Failed to create ComponentRegisterMaster.cpp" << std::endl;
        return false;
    }

    implFile << "#include \"ComponentRegisterMaster.h\"\n";
    for (const auto& comp : components)
    {
        implFile << "#include \"" << comp.className << "_Barebones.h\"\n";
    }
    implFile << "\n";
    implFile << "namespace dae\n";
    implFile << "{\n";

    for (const auto& comp : components)
    {
        implFile << "    void " << comp.className << "_Barebones::RegisterComponentStatic()\n";
        implFile << "    {\n";
        implFile << "    }\n\n";
    }

    implFile << "    const std::vector<ComponentMetadata>& GetAllComponentMetadata()\n";
    implFile << "    {\n";
    implFile << "        static const std::vector<ComponentMetadata> metadata = {\n";

    for (size_t i = 0; i < components.size(); ++i)
    {
        const auto& comp = components[i];
        implFile << "            ComponentMetadata{\n";
        implFile << "                \"" << comp.className << "\",\n";
        implFile << "                \"" << comp.className << "_Barebones\",\n";
        implFile << "                {\n";

        for (size_t j = 0; j < comp.exposedProperties.size(); ++j)
        {
            const auto& prop = comp.exposedProperties[j];
            implFile << "                    PropertyMetadata{\n";
            implFile << "                        \"" << prop.type << "\",\n";
            implFile << "                        \"" << prop.name << "\",\n";
            implFile << "                        \"" << prop.displayName << "\",\n";
            implFile << "                        \"" << prop.tooltip << "\"\n";
            implFile << "                    }" << (j < comp.exposedProperties.size() - 1 ? "," : "") << "\n";
        }

        implFile << "                }\n";
        implFile << "            }" << (i < components.size() - 1 ? "," : "") << "\n";
    }

    implFile << "        };\n";
    implFile << "        return metadata;\n";
    implFile << "    }\n\n";

    implFile << "    void RegisterAllComponents()\n";
    implFile << "    {\n";
    for (const auto& comp : components)
    {
        if (comp.className != "Component")
        {
            implFile << "        {\n";
            implFile << "            " << comp.className << "_Barebones instance;\n";
            implFile << "            instance.RegisterComponentStatic();\n";
            implFile << "        }\n";
        }
    }
    implFile << "    }\n";
    implFile << "}\n";
    implFile.close();
    return true;
}

bool ComponentParser::GenerateBarebonesGameObject(const std::string& gameObjectPath, const std::string& outputDir)
{
    std::vector<Property> exposedProps;

    std::ifstream file(gameObjectPath);
    if (!file.is_open())
    {
        std::cerr << "Failed to open GameObject.h: " << gameObjectPath << std::endl;
        return false;
    }

    std::string line;
    std::string prevLine;
    while (std::getline(file, line))
    {
        if (prevLine.find("EXPOSE_TO_EDITOR") != std::string::npos)
        {
            Property prop;
            if (ExtractPropertyFromLine(line, prop))
            {
                size_t openParen = prevLine.find("(");
                size_t closeParen = prevLine.find(")");
                if (openParen != std::string::npos && closeParen != std::string::npos)
                {
                    std::string metadata = prevLine.substr(openParen + 1, closeParen - openParen - 1);
                    size_t firstQuote = metadata.find("\"");
                    if (firstQuote != std::string::npos)
                    {
                        size_t secondQuote = metadata.find("\"", firstQuote + 1);
                        if (secondQuote != std::string::npos)
                        {
                            prop.displayName = metadata.substr(firstQuote + 1, secondQuote - firstQuote - 1);
                        }
                    }
                    size_t thirdQuote = metadata.find("\"", firstQuote + 1);
                    if (thirdQuote != std::string::npos)
                    {
                        thirdQuote = metadata.find("\"", thirdQuote + 1);
                        if (thirdQuote != std::string::npos)
                        {
                            size_t fourthQuote = metadata.find("\"", thirdQuote + 1);
                            if (fourthQuote != std::string::npos)
                            {
                                prop.tooltip = metadata.substr(thirdQuote + 1, fourthQuote - thirdQuote - 1);
                            }
                        }
                    }
                }
                if (prop.displayName.empty())
                {
                    prop.displayName = prop.name;
                }
                exposedProps.push_back(prop);
            }
        }
        prevLine = line;
    }
    file.close();

    std::string headerPath = (fs::path(outputDir) / "GameObject_Barebones.h").string();
    std::ofstream headerFile(headerPath);
    if (!headerFile.is_open())
    {
        std::cerr << "Failed to create GameObject_Barebones.h" << std::endl;
        return false;
    }

    headerFile << "#ifndef GAMEOBJECT_BAREBONES_H\n";
    headerFile << "#define GAMEOBJECT_BAREBONES_H\n\n";
    headerFile << "#include <glm/glm.hpp>\n";
    headerFile << "#include <string>\n";
    headerFile << "#include <vector>\n";
    headerFile << "#include <algorithm>\n\n";
    headerFile << "namespace dae\n";
    headerFile << "{\n";
    headerFile << "    class GameObject_Barebones\n";
    headerFile << "    {\n";
    headerFile << "    public:\n";
    headerFile << "        static inline int s_nextId = 0;\n";
    headerFile << "        \n";
    headerFile << "        GameObject_Barebones() : m_id(s_nextId++) {}\n";
    headerFile << "        ~GameObject_Barebones() = default;\n\n";
    headerFile << "        int GetId() const noexcept { return m_id; }\n\n";

    for (const auto& prop : exposedProps)
    {
        std::string accessorName = prop.name;
        if (!accessorName.empty())
        {
            accessorName[0] = static_cast<char>(std::toupper(accessorName[0]));
        }
        headerFile << "        const " << prop.type << "& Get" << accessorName << "() const noexcept { return m_" << prop.name << "; }\n";
        headerFile << "        void Set" << accessorName << "(const " << prop.type << "& value) { m_" << prop.name << " = value; }\n";
        headerFile << "\n";
    }

    headerFile << "        GameObject_Barebones* GetParent() const noexcept { return m_parent; }\n";
    headerFile << "        void SetParent(GameObject_Barebones* parent) { m_parent = parent; }\n\n";
    headerFile << "        const std::vector<GameObject_Barebones*>& GetChildren() const noexcept { return m_children; }\n";
    headerFile << "        std::vector<GameObject_Barebones*>& GetChildren() noexcept { return m_children; }\n\n";
    headerFile << "        void AddChild(GameObject_Barebones* child)\n";
    headerFile << "        {\n";
    headerFile << "            if (!child) return;\n";
    headerFile << "            if (child->m_parent) child->m_parent->RemoveChild(child);\n";
    headerFile << "            m_children.push_back(child);\n";
    headerFile << "            child->m_parent = this;\n";
    headerFile << "        }\n\n";
    headerFile << "        void RemoveChild(GameObject_Barebones* child)\n";
    headerFile << "        {\n";
    headerFile << "            auto it = std::find(m_children.begin(), m_children.end(), child);\n";
    headerFile << "            if (it != m_children.end())\n";
    headerFile << "            {\n";
    headerFile << "                m_children.erase(it);\n";
    headerFile << "                child->m_parent = nullptr;\n";
    headerFile << "            }\n";
    headerFile << "        }\n\n";
    headerFile << "        const std::vector<void*>& GetComponents() const noexcept { return m_components; }\n";
    headerFile << "        void AddComponent(void* component)\n";
    headerFile << "        {\n";
    headerFile << "            if (!component) return;\n";
    headerFile << "            m_components.push_back(component);\n";
    headerFile << "        }\n\n";
    headerFile << "        void RemoveComponent(void* component)\n";
    headerFile << "        {\n";
    headerFile << "            auto it = std::find(m_components.begin(), m_components.end(), component);\n";
    headerFile << "            if (it != m_components.end())\n";
    headerFile << "            {\n";
    headerFile << "                m_components.erase(it);\n";
    headerFile << "                delete component;\n";
    headerFile << "            }\n";
    headerFile << "        }\n\n";
    headerFile << "        void ClearComponents()\n";
    headerFile << "        {\n";
    headerFile << "            for (auto comp : m_components)\n";
    headerFile << "            {\n";
    headerFile << "                delete comp;\n";
    headerFile << "            }\n";
    headerFile << "            m_components.clear();\n";
    headerFile << "        }\n\n";

    headerFile << "    private:\n";
    headerFile << "        GameObject_Barebones(const GameObject_Barebones&) = delete;\n";
    headerFile << "        GameObject_Barebones& operator=(const GameObject_Barebones&) = delete;\n\n";
    headerFile << "        int m_id;\n";

    for (const auto& prop : exposedProps)
    {
        headerFile << "        " << prop.type << " m_" << prop.name << "{};\n";
    }

    headerFile << "        GameObject_Barebones* m_parent = nullptr;\n";
    headerFile << "        std::vector<GameObject_Barebones*> m_children;\n";
    headerFile << "        std::vector<void*> m_components;\n";

    headerFile << "    };\n";
    headerFile << "}\n\n";
    headerFile << "#endif // GAMEOBJECT_BAREBONES_H\n";

    headerFile.close();
    return true;
}
