#pragma once

#include <vector>
#include <memory>
#include <map>
#include <string>
#include "../gencomponents/GameObject_Barebones.h"

class EditorScene
{
public:
    dae::GameObject_Barebones* CreateGameObject(const std::string& name, dae::GameObject_Barebones* parent = nullptr);
    void DeleteGameObject(dae::GameObject_Barebones* obj);

    std::vector<dae::GameObject_Barebones*> GetRootObjects() const;
    std::vector<std::unique_ptr<dae::GameObject_Barebones>>& GetAllObjects() { return m_objects; }
    const std::vector<std::unique_ptr<dae::GameObject_Barebones>>& GetAllObjects() const { return m_objects; }

    void RegisterComponentType(dae::GameObject_Barebones* gameObject, void* componentPtr, const std::string& typeName);
    void UnregisterComponentType(dae::GameObject_Barebones* gameObject, void* componentPtr);
    std::string GetComponentType(dae::GameObject_Barebones* gameObject, void* componentPtr) const;

private:
    std::vector<std::unique_ptr<dae::GameObject_Barebones>> m_objects;
    std::map<dae::GameObject_Barebones*, std::map<void*, std::string>> m_componentTypes;
};
