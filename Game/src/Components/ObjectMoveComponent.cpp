#include "ObjectMoveComponent.h"
#include "GameObject.h"
#include "Timing.h"
#include <map>

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

bool dae::ObjectMoveComponent::Deserialize(const std::map<std::string, std::string>& properties, std::string& errorMessage)
{
	(void)properties;
	(void)errorMessage;
	return true;
}

void dae::ObjectMoveComponent::MoveObject(glm::vec2& direction, float speed)
{
	auto parent = GetParent();
	auto currentPos = parent->GetLocalPosition();
	currentPos += direction * speed * Timing::GetInstance().GetDeltaTime();
	parent->SetLocalPosition(currentPos);
}