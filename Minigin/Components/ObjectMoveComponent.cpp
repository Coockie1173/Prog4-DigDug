#include "ObjectMoveComponent.h"
#include "GameObject.h"
#include "Timing.h"

dae::ObjectMoveComponent::ObjectMoveComponent(GameObject* Parent) : Component(Parent)
{
}

void dae::ObjectMoveComponent::Update()
{
}

void dae::ObjectMoveComponent::LateUpdate()
{
}

void dae::ObjectMoveComponent::Init()
{
}

void dae::ObjectMoveComponent::MoveObject(glm::vec2& direction, float speed)
{
	auto parent = GetParent();
	auto currentPos = parent->GetLocalPosition();
	currentPos += direction * speed * Timing::GetInstance().GetDeltaTime();
	parent->SetLocalPosition(currentPos);
}
