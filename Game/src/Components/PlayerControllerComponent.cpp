#include <Components/PlayerControllerComponent.h>

PlayerControllerComponent::PlayerControllerComponent(dae::GameObject* Parent) : Component(Parent)
{
}

void PlayerControllerComponent::Update()
{
}

void PlayerControllerComponent::LateUpdate()
{
}

void PlayerControllerComponent::Init()
{
}

bool PlayerControllerComponent::Deserialize(const std::map<std::string, std::string>& properties, std::string& errorMessage)
{
	return false;
}
