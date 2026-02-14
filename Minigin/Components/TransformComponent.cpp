#include "TransformComponent.h"

TransformComponent::TransformComponent(GameObject* Parent) : Component(Parent)
{
	m_myType = ComponentType::TYPE_TRANSFORM;

	glm::vec2 Empty{ 0 };
	m_position = Empty;
}

void TransformComponent::SetPosition(float x, float y)
{
	m_position.x = x;
	m_position.y = y;
}

void TransformComponent::SetPosition(const glm::vec2& position)
{
	m_position = position;
}

void TransformComponent::Update()
{
}

void TransformComponent::Init()
{
}

void TransformComponent::LateUpdate()
{
}
