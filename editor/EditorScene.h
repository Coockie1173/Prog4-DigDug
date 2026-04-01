#pragma once

#include <vector>
#include <memory>
#include "../gencomponents/GameObject_Barebones.h"

class EditorScene
{
public:
    dae::GameObject_Barebones* CreateGameObject(const std::string& name, dae::GameObject_Barebones* parent = nullptr);
    void DeleteGameObject(dae::GameObject_Barebones* obj);
    
    std::vector<dae::GameObject_Barebones*> GetRootObjects() const;
    std::vector<std::unique_ptr<dae::GameObject_Barebones>>& GetAllObjects() { return m_objects; }
    const std::vector<std::unique_ptr<dae::GameObject_Barebones>>& GetAllObjects() const { return m_objects; }

private:
    std::vector<std::unique_ptr<dae::GameObject_Barebones>> m_objects;
};
