#include <SceneDeserializer.h>
#include <MBINSerializer.h>
#include <ComponentTypeMap.h>
#include <fstream>
#include <format>

namespace dae
{
    std::unique_ptr<SceneData> SceneDeserializer::DeserializeFromFile(
        const std::string& filepath,
        std::string& errorMessage
    )
    {
        if (filepath.empty())
        {
            errorMessage = "Filepath cannot be empty";
            return nullptr;
        }

        std::ifstream file(filepath, std::ios::binary);

        if (!file.is_open())
        {
            errorMessage = std::format("Failed to open file for reading: {}", filepath);
            return nullptr;
        }

        // Read and validate magic number
        char magic[5] = {};
        file.read(magic, 4);

        if (std::string_view(magic, 4) != "MBIN")
        {
            errorMessage = "Invalid file format - magic number mismatch";
            return nullptr;
        }

        // Read and validate version
        uint32_t version = 0;
        serialization::ReadData(file, version);

        if (version != 1)
        {
            errorMessage = std::format("Unsupported file version: {}", version);
            return nullptr;
        }

        auto sceneData = std::make_unique<SceneData>();

        try
        {
            // Deserialize input bindings
            uint32_t bindingCount = 0;
            serialization::ReadData(file, bindingCount);

            for (uint32_t i = 0; i < bindingCount; ++i)
            {
                std::string actionName = serialization::ReadString(file);
                uint8_t deviceType = 0;
                serialization::ReadData(file, deviceType);

                InputBinding binding{};
                binding.actionName = std::move(actionName);
                binding.deviceType = static_cast<InputDeviceType>(deviceType);

                if (binding.deviceType == InputDeviceType::Keyboard)
                {
                    serialization::ReadData(file, binding.keyCode);
                }
                else if (binding.deviceType == InputDeviceType::Gamepad)
                {
                    serialization::ReadData(file, binding.gamepadIndex);
                    uint8_t buttonIndex = 0;
                    serialization::ReadData(file, buttonIndex);
                    binding.gamepadButton = static_cast<dae::GamepadButton>(buttonIndex);
                }

                sceneData->inputBindings[binding.actionName] = std::move(binding);
            }

            // Deserialize game objects
            uint32_t objectCount = 0;
            serialization::ReadData(file, objectCount);

            for (uint32_t i = 0; i < objectCount; ++i)
            {
                GameObjectData objData;

                serialization::ReadData(file, objData.id);
                objData.name = serialization::ReadString(file);
                serialization::ReadData(file, objData.parentId);
                serialization::ReadData(file, objData.worldPosition.x);
                serialization::ReadData(file, objData.worldPosition.y);
                serialization::ReadData(file, objData.isDebug);

                uint32_t componentCount = 0;
                serialization::ReadData(file, componentCount);

                for (uint32_t j = 0; j < componentCount; ++j)
                {
                    auto component = std::make_unique<ComponentInstance>();

                    serialization::ReadData(file, component->componentTypeHash);
                    component->componentType = GetComponentTypeFromHash(component->componentTypeHash);
                    component->componentName = serialization::ReadString(file);
                    component->properties = serialization::ReadMap(file);

                    objData.components.push_back(std::move(component));
                }

                sceneData->gameObjects.push_back(std::move(objData));
            }

            file.close();
            return sceneData;
        }
        catch (const std::exception& e)
        {
            file.close();
            errorMessage = std::format("Error during deserialization: {}", e.what());
            return nullptr;
        }
    }
}
