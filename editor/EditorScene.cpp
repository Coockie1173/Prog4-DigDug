#include "EditorScene.h"
#include "EditorUI.h"
#include <algorithm>

dae::GameObject_Barebones* EditorScene::CreateGameObject(const std::string& name, dae::GameObject_Barebones* parent)
{
    auto obj = std::make_unique<dae::GameObject_Barebones>();
    obj->SetObjectName(name);
    dae::GameObject_Barebones* ptr = obj.get();
    m_objects.push_back(std::move(obj));
    if (parent)
        parent->AddChild(ptr);
    return ptr;
}

void EditorScene::DeleteGameObject(dae::GameObject_Barebones* obj)
{
    if (!obj) return;
    if (obj->GetParent())
        obj->GetParent()->RemoveChild(obj);

    // Clean up component type mappings for this object
    m_componentTypes.erase(obj);

    auto it = std::find_if(m_objects.begin(), m_objects.end(),
        [obj](const std::unique_ptr<dae::GameObject_Barebones>& ptr) { return ptr.get() == obj; });
    if (it != m_objects.end())
        m_objects.erase(it);
}

std::vector<dae::GameObject_Barebones*> EditorScene::GetRootObjects() const
{
    std::vector<dae::GameObject_Barebones*> roots;
    for (const auto& obj : m_objects)
    {
        if (obj->GetParent() == nullptr)
            roots.push_back(obj.get());
    }
    return roots;
}

void EditorScene::RegisterComponentType(dae::GameObject_Barebones* gameObject, const dae::ComponentInstance* componentPtr, unsigned int typeHash)
{
    if (!gameObject || !componentPtr) return;
    m_componentTypes[gameObject][componentPtr] = typeHash;
}

void EditorScene::UnregisterComponentType(dae::GameObject_Barebones* gameObject, const dae::ComponentInstance* componentPtr)
{
    if (!gameObject || !componentPtr) return;

    auto it = m_componentTypes.find(gameObject);
    if (it != m_componentTypes.end())
    {
        it->second.erase(componentPtr);
        if (it->second.empty())
            m_componentTypes.erase(it);
    }
}

unsigned int EditorScene::GetComponentTypeHash(dae::GameObject_Barebones* gameObject, const dae::ComponentInstance* componentPtr) const
{
    if (!gameObject || !componentPtr) return 0;

    auto it = m_componentTypes.find(gameObject);
    if (it != m_componentTypes.end())
    {
        auto compIt = it->second.find(componentPtr);
        if (compIt != it->second.end())
            return compIt->second;
    }

    return 0;
}