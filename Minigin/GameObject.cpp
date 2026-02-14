#include <string>
#include "GameObject.h"
#include "ResourceManager.h"
#include "Renderer.h"
#include "Components/Component.h"
#include "Components/TransformComponent.h"
#include "Components/RenderComponent.h"
#include <iostream>

dae::GameObject::GameObject(std::string Name) : m_objectName(Name)
{	
    AddBaseComponents();
}

dae::GameObject::GameObject(std::string Name, float PosX, float PosY) : m_objectName(Name)
{
    AddBaseComponents();
    m_transform->SetPosition(PosX, PosY);
}

dae::GameObject::GameObject(std::string Name, glm::vec2 Position) : m_objectName(Name)
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
}

void dae::GameObject::Update()
{
    for (auto& c : m_attachedComponents)
    {
        c->Update();
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

const glm::vec2& dae::GameObject::GetPosition()
{
    return m_transform->GetPosition();
}
