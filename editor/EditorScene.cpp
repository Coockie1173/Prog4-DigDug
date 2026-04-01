#include "EditorScene.h"
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
