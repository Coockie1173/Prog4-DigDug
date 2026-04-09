#include "EditorSerialization.h"
#include <fstream>
#include <cstring>
#include <format>
#include <SDL3/SDL.h>
#include "../Shared/include/MBINSerializer.h"
#include "../Shared/include/ComponentTypeMap.h"

EditorSerialization::EditorSerialization()
{
}

void EditorSerialization::OpenSaveDialog(EditorScene* scene, InputBindingEditor* inputBindingEditor)
{
    m_sceneToSave = scene;
    m_inputBindingToSave = inputBindingEditor;

    SDL_DialogFileFilter filters[] = {
        { "Binary Scene", "mbin" },
        { "All Files", "*" }
    };

    SDL_ShowSaveFileDialog(SaveFileDialogCallback, this, nullptr, filters, 2, "scene.mbin");
}

void EditorSerialization::OpenLoadDialog(EditorScene* scene, InputBindingEditor* inputBindingEditor)
{
    m_sceneToLoad = scene;
    m_inputBindingToLoad = inputBindingEditor;

    SDL_DialogFileFilter filters[] = {
        { "Binary Scene", "mbin" },
        { "All Files", "*" }
    };

    SDL_ShowOpenFileDialog(LoadFileDialogCallback, this, nullptr, filters, 2, nullptr, false);
}

void EditorSerialization::SaveFileDialogCallback(void* userdata, const char* const* filelist, int)
{
    EditorSerialization* pThis = static_cast<EditorSerialization*>(userdata);

    if (filelist && filelist[0])
    {
        pThis->SaveSceneToFile(filelist[0]);
    }
}

void EditorSerialization::LoadFileDialogCallback(void* userdata, const char* const* filelist, int)
{
    EditorSerialization* pThis = static_cast<EditorSerialization*>(userdata);

    if (filelist && filelist[0])
    {
        pThis->LoadSceneFromFile(filelist[0]);
    }
}

void EditorSerialization::SaveSceneToFile(const std::string& filepath)
{
    if (!m_sceneToSave || !m_inputBindingToSave)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Scene or input bindings not available", nullptr);
        return;
    }

    if (filepath.empty())
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Filepath cannot be empty", nullptr);
        return;
    }

    std::ofstream file(filepath, std::ios::binary);

    if (!file.is_open())
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Failed to open file for writing", nullptr);
        return;
    }

    constexpr std::string_view magic = "MBIN";
    file.write(magic.data(), 4);

    constexpr uint32_t version = 1;
    dae::serialization::WriteData(file, version);

    const auto& bindings = m_inputBindingToSave->GetBindings();
    auto bindingCount = static_cast<uint32_t>(bindings.size());
    dae::serialization::WriteData(file, bindingCount);

    for (const auto& [actionName, binding] : bindings)
    {
        dae::serialization::WriteString(file, actionName);

        auto deviceType = static_cast<uint8_t>(binding.deviceType);
        dae::serialization::WriteData(file, deviceType);

        if (binding.deviceType == dae::InputDeviceType::Keyboard)
        {
            dae::serialization::WriteData(file, binding.keyCode);
        }
        else if (binding.deviceType == dae::InputDeviceType::Gamepad)
        {
            dae::serialization::WriteData(file, binding.gamepadIndex);
            auto buttonIndex = static_cast<uint8_t>(binding.gamepadButton);
            dae::serialization::WriteData(file, buttonIndex);
        }
    }

    const auto& allObjects = m_sceneToSave->GetAllObjects();

    std::map<int, dae::GameObject_Barebones*> idToObjectMap;
    std::vector<dae::GameObject_Barebones*> rootObjects = m_sceneToSave->GetRootObjects();

    for (const auto& obj : allObjects)
    {
        idToObjectMap[obj->GetId()] = obj.get();
    }

    auto objectCount = static_cast<uint32_t>(allObjects.size());
    dae::serialization::WriteData(file, objectCount);

    for (const auto& obj : allObjects)
    {
        auto id = static_cast<uint32_t>(obj->GetId());
        dae::serialization::WriteData(file, id);

        dae::serialization::WriteString(file, obj->GetObjectName());

        dae::GameObject_Barebones* parent = obj->GetParent();
        auto parentId = static_cast<int32_t>(parent ? parent->GetId() : -1);
        dae::serialization::WriteData(file, parentId);

        auto worldPos = obj->GetWorldPosition();
        dae::serialization::WriteData(file, worldPos.x);
        dae::serialization::WriteData(file, worldPos.y);

        bool isDebug = obj->GetIsDebugData();
        dae::serialization::WriteData(file, isDebug);

        const auto& components = obj->GetComponents();
        auto componentCount = static_cast<uint32_t>(components.size());
        dae::serialization::WriteData(file, componentCount);

        for (const auto& component : components)
        {
            if (component)
            {
                dae::serialization::WriteData(file, component->componentTypeHash);
                dae::serialization::WriteString(file, component->componentName);
                dae::serialization::WriteMap(file, component->properties);
            }
        }
    }

    file.close();

    auto message = std::format("Scene saved successfully to: {}", filepath);
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Success", message.c_str(), nullptr);
}

void EditorSerialization::LoadSceneFromFile(const std::string& filepath)
{
    if (!m_sceneToLoad || !m_inputBindingToLoad)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Scene or input bindings not available", nullptr);
        return;
    }

    if (filepath.empty())
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Filepath cannot be empty", nullptr);
        return;
    }

    std::ifstream file(filepath, std::ios::binary);

    if (!file.is_open())
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Failed to open file for reading", nullptr);
        return;
    }

    char magic[5] = {};
    file.read(magic, 4);

    if (std::string_view(magic, 4) != "MBIN")
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Invalid file format", nullptr);
        return;
    }

    uint32_t version = 0;
    dae::serialization::ReadData(file, version);

    if (version != 1)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Unsupported file version", nullptr);
        return;
    }

    uint32_t bindingCount = 0;
    dae::serialization::ReadData(file, bindingCount);

    for (uint32_t i = 0; i < bindingCount; ++i)
    {
        std::string actionName = dae::serialization::ReadString(file);
        uint8_t deviceType = 0;
        dae::serialization::ReadData(file, deviceType);

        dae::InputBinding binding{};
        binding.actionName = std::move(actionName);
        binding.deviceType = static_cast<dae::InputDeviceType>(deviceType);

        if (binding.deviceType == dae::InputDeviceType::Keyboard)
        {
            dae::serialization::ReadData(file, binding.keyCode);
        }
        else if (binding.deviceType == dae::InputDeviceType::Gamepad)
        {
            dae::serialization::ReadData(file, binding.gamepadIndex);
            uint8_t buttonIndex = 0;
            dae::serialization::ReadData(file, buttonIndex);
            binding.gamepadButton = static_cast<dae::GamepadButton>(buttonIndex);
        }

        m_inputBindingToLoad->GetBindings()[binding.actionName] = binding;
    }

    uint32_t objectCount = 0;
    dae::serialization::ReadData(file, objectCount);

    std::map<uint32_t, dae::GameObject_Barebones*> idToObjectMap;

    for (uint32_t i = 0; i < objectCount; ++i)
    {
        uint32_t id = 0;
        dae::serialization::ReadData(file, id);

        std::string name = dae::serialization::ReadString(file);

        int32_t parentId = 0;
        dae::serialization::ReadData(file, parentId);

        float worldPosX = 0.0f, worldPosY = 0.0f;
        dae::serialization::ReadData(file, worldPosX);
        dae::serialization::ReadData(file, worldPosY);

        bool isDebug = false;
        dae::serialization::ReadData(file, isDebug);

        dae::GameObject_Barebones* parent = nullptr;
        if (parentId != -1 && idToObjectMap.contains(parentId))
        {
            parent = idToObjectMap[parentId];
        }

        dae::GameObject_Barebones* obj = m_sceneToLoad->CreateGameObject(name, parent);
        obj->SetWorldPosition(glm::vec2(worldPosX, worldPosY));
        obj->SetIsDebugData(isDebug);

        idToObjectMap[id] = obj;

        uint32_t componentCount = 0;
        dae::serialization::ReadData(file, componentCount);

        for (uint32_t j = 0; j < componentCount; ++j)
        {
            unsigned int componentTypeHash = 0;
            dae::serialization::ReadData(file, componentTypeHash);
            std::string componentName = dae::serialization::ReadString(file);
            std::map<std::string, std::string> properties = dae::serialization::ReadMap(file);

            auto component = std::make_unique<dae::ComponentInstance>();
            component->componentTypeHash = componentTypeHash;
            component->componentType = dae::GetComponentTypeFromHash(componentTypeHash);
            component->componentName = std::move(componentName);
            component->properties = std::move(properties);

            const auto* handlePtr = component.get();
            obj->AddComponent(std::move(component));
            m_sceneToLoad->RegisterComponentType(obj, handlePtr, componentTypeHash);
        }
    }

    file.close();

    auto message = std::format("Scene loaded successfully from: {}", filepath);
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Success", message.c_str(), nullptr);
}
