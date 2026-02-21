#include <string>
#include "GameObject.h"
#include "ResourceManager.h"
#include "Renderer.h"
#include "Components/Component.h"
#include "Components/TransformComponent.h"
#include "Components/RenderComponent.h"
#include <iostream>

dae::GameObject::GameObject(std::string Name, GameObject* Parent, bool IsDebugObject) 
    : m_parent(Parent), m_objectName(Name), m_isDebugData(IsDebugObject)
{	
    glm::vec2 Empty{ 0 };
    m_localPosition = Empty;
    m_worldPosition = Empty;
}

dae::GameObject::GameObject(std::string Name, float PosX, float PosY, GameObject* Parent, bool IsDebugObject)
    : m_parent(Parent), m_objectName(Name), m_isDebugData(IsDebugObject)
{
    SetPosition(PosX, PosY);
}

dae::GameObject::GameObject(std::string Name, glm::vec2 Position, GameObject* Parent, bool IsDebugObject)
    : m_parent(Parent), m_objectName(Name), m_isDebugData(IsDebugObject)
{
    SetPosition(Position);
}

dae::GameObject::~GameObject() = default;

void dae::GameObject::SetPosition(float x, float y)
{
    if (m_localPosition.x == x && m_localPosition.y == y)
    {
        return;
    }

    m_localPosition.x = x;
    m_localPosition.y = y;
    m_positionIsDirty = true;
}

void dae::GameObject::SetPosition(const glm::vec2& position)
{
    if (position == m_localPosition)
    {
        return;
    }

    m_localPosition = position;
    m_positionIsDirty = true;
}

void dae::GameObject::SetParent(GameObject* Parent, [[maybe_unused]]bool KeepWorldPos)
{
    //TODO: add position stuff
    if (IsChildOf(Parent) || Parent == this || m_parent == Parent)
    {
        return;
    }
    if (m_parent)
    {
        m_parent->RemoveChild(this);
    }
    m_parent = Parent;
    if (m_parent)
    {
        m_parent->AddChild(this);
    }
}

void dae::GameObject::AddChild(GameObject* Child)
{
    m_children.push_back(Child);
}

void dae::GameObject::RemoveChild(GameObject* Child)
{
    auto it = std::find(m_children.begin(), m_children.end(), Child);
    if (it != m_children.end())
    {
        m_children.erase(it);
    }
}

bool dae::GameObject::IsChildOf(GameObject* Parent)
{
    auto it = std::find(Parent->m_children.begin(), Parent->m_children.end(), this);
    return it != m_children.end();
}

void dae::GameObject::Init()
{
    for (auto& c : m_attachedComponents)
    {
        c->Init();
    }
}

void dae::GameObject::Update()
{
    for (auto& c : m_attachedComponents)
    {
        c->Update();
    }
}

void dae::GameObject::LateUpdate()
{
    for (auto& c : m_attachedComponents)
    {
        c->LateUpdate();
    }
}

void dae::GameObject::Render() const
{
    if (m_renderer != nullptr)
    {
        m_renderer->Render();
    }
}

const glm::vec2& dae::GameObject::GetPosition() const noexcept
{
    return m_localPosition;
}

template<typename T>
inline T* dae::GameObject::GetComponent()
{
    static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");

    for (auto& C : m_attachedComponents)
    {
        if (auto CastC = dynamic_cast<T*>(C))
        {
            return CastC;
        }
    }

    return nullptr;
}

template<typename T>
void dae::GameObject::GetComponentsOfType(std::vector<T*>& ReturnedComponents)
{
    static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");

    for (auto& C : m_attachedComponents)
    {
        if (auto CastC = dynamic_cast<T*>(C))
        {
            ReturnedComponents.push_back(CastC);
        }
    }
}

template<typename T>
void dae::GameObject::GetComponentIDsOfType(std::vector<int>& ReturnedComponentIDs)
{
    int idx = 0;
    for (auto& C : m_attachedComponents)
    {
        if (auto CastC = dynamic_cast<T*>(C))
        {
            ReturnedComponentIDs.push_back(idx);
        }
        idx++;
    }
}

template<typename T>
bool dae::GameObject::RemoveComponent()
{
    static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");

    int idx = 0;
    for (auto& C : m_attachedComponents)
    {
        if (auto CastC = dynamic_cast<T*>(C))
        {
            m_attachedComponents.erase(m_attachedComponents.begin() + idx);
            return true;
        }
        idx++;
    }

    return false;
}

void dae::GameObject::RemoveComponentAtID(int ID)
{
    m_attachedComponents.erase(m_attachedComponents.begin() + ID);
}

template<typename T>
bool dae::GameObject::HasComponent()
{
    static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");

    T* GC = GetComponent<T>();

    return GC != nullptr;
}
