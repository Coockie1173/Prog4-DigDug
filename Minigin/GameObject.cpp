#include <string>
#include "GameObject.h"
#include "ResourceManager.h"
#include "Renderer.h"
#include "Components/Component.h"
#include "Components/TransformComponent.h"
#include <iostream>

dae::GameObject::GameObject()
{	
    AddBaseComponents();
}

dae::GameObject::GameObject(float PosX, float PosY)
{
    AddBaseComponents();
    m_transform->SetPosition(PosX, PosY);
}

dae::GameObject::GameObject(glm::vec2 Position)
{
    AddBaseComponents();
    m_transform->SetPosition(Position);
}

dae::GameObject::~GameObject() = default;

//Used for adding the base components to a gameobject. DO NOT add more than needed future me.
void dae::GameObject::AddBaseComponents()
{
    AddComponent<TransformComponent>(this);
}

void dae::GameObject::Init()
{
}

void dae::GameObject::Update()
{

}

void dae::GameObject::Render() const
{
	//const auto& pos = m_transform.GetPosition();
	//Renderer::GetInstance().RenderTexture(*m_texture, pos.x, pos.y);
}

void dae::GameObject::SetPosition(float x, float y)
{
	m_transform->SetPosition(x, y, 0.0f);
}

const glm::vec2& dae::GameObject::GetPosition()
{
    return m_transform->GetPosition();
}

template <typename T, typename... Args>
std::shared_ptr<T> dae::GameObject::AddComponent(Args&&... args)
{
    static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");

    auto component = std::make_shared<T>(std::forward<Args>(args)...);
    m_attachedComponents.push_back(component);

    if (component->GetType() == ComponentType::TYPE_TRANSFORM)
    {
        if (m_transform != nullptr)
        {
            m_attachedComponents.pop_back();
            static_assert(false, "Cannot have multiple transforms on the same object");
            return;
        }
        m_transform = std::static_pointer_cast<TransformComponent>(component);
    }

    return component;
}
