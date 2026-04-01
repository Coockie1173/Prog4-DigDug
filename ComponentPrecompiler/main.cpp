#include "ComponentParser.h"
#include <iostream>

int main(int argc, char* argv[])
{
    if (argc < 4)
    {
        std::cerr << "Usage: ComponentPrecompiler <component_dir> <gameobject_path> <output_dir>" << std::endl;
        return 1;
    }

    ComponentParser parser;
    if (parser.ParseComponentDirectory(argv[1], argv[2], argv[3]))
    {
        std::cout << "Component precompilation completed successfully." << std::endl;
        return 0;
    }

    std::cerr << "Component precompilation failed." << std::endl;
    return 1;
}
