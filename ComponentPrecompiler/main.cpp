#include "ComponentParser.h"
#include <iostream>

int main(int argc, char* argv[])
{
    if (argc < 4)
    {
        std::cerr << "Usage: ComponentPrecompiler <gameobject_path> <output_dir> <component_dir> [component_dir...]" << std::endl;
        return 1;
    }

    ComponentParser parser;
    std::vector<std::string> componentDirs;
    for (int i = 3; i < argc; ++i)
    {
        componentDirs.emplace_back(argv[i]);
    }

    if (parser.ParseComponentDirectories(componentDirs, argv[1], argv[2]))
    {
        std::cout << "Component precompilation completed successfully." << std::endl;
        return 0;
    }

    std::cerr << "Component precompilation failed." << std::endl;
    return 1;
}
