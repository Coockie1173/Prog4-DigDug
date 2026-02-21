#include <string>
#include "GameObject.h"
#include "ResourceManager.h"
#include "Renderer.h"
#include "Components/Component.h"
#include "Components/TransformComponent.h"
#include "Components/RenderComponent.h"
#include <iostream>

dae::GameObject::GameObject(std::string Name, bool IsDebugObject) : m_objectName(Name), m_isDebugData(IsDebugObject)
{	
    AddBaseComponents();
}

dae::GameObject::GameObject(std::string Name, float PosX, float PosY, bool IsDebugObject) : m_objectName(Name), m_isDebugData(IsDebugObject)
{
    AddBaseComponents();
    m_transform->SetPosition(PosX, PosY);
}

dae::GameObject::GameObject(std::string Name, glm::vec2 Position, bool IsDebugObject) : m_objectName(Name), m_isDebugData(IsDebugObject)
{
    AddBaseComponents();
    m_transform->SetPosition(Position);
}

dae::GameObject::~GameObject() = default;

//Used for adding the base components to a gameobject. DO NOT add more than needed future me.
void dae::GameObject::AddBaseComponents()
{
    AddComponent<TransformComponent>();
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

void dae::GameObject::SetPosition(float x, float y)
{
	m_transform->SetPosition(x, y);
}

const glm::vec2& dae::GameObject::GetPosition() const noexcept
{
    return m_transform->GetPosition();
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

template<typename T>
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
