#include <Components/PumpComponent.h>

#include <ComponentFactoryRegistry.h>
#include <ComponentTypeMap.h>
#include <Components/ObjectMoveComponent.h>
#include <Components/PlayerControllerComponent.h>
#include <GameObject.h>
#include <Timing.h>

#include <algorithm>
#include <cmath>

namespace
{
	const bool PumpComponentRegistered = dae::RegisterComponentFactoryFor<dae::PumpComponent>(dae::HASH_PumpComponent);
}

dae::PumpComponent::PumpComponent(GameObject* Parent) : Component(Parent)
{
}

void dae::PumpComponent::Configure(PlayerControllerComponent* controller, GameObject* tailObject, glm::vec2 direction, float travelDistance, float speed)
{
	m_pController = controller;
	m_pTailObject = tailObject;
	m_Direction = glm::length(direction) > 0.0f ? glm::normalize(direction) : glm::vec2{ 1.0f, 0.0f };
	m_TravelDistance = travelDistance;
	m_Speed = speed;
	m_TravelledDistance = 0.0f;
	m_IsConfigured = true;
}

void dae::PumpComponent::Update()
{
	if (!m_IsConfigured || m_IsFinished)
	{
		return;
	}

	if (m_pMoveComponent == nullptr)
	{
		m_pMoveComponent = GetParent()->GetComponent<ObjectMoveComponent>();
	}

	if (m_pMoveComponent == nullptr)
	{
		FinishPump();
		return;
	}

	m_pMoveComponent->MoveObject(m_Direction, m_Speed);
	m_TravelledDistance += m_Speed * Timing::GetInstance().GetDeltaTime();

	if (m_TravelledDistance >= m_TravelDistance)
	{
		FinishPump();
	}
}

void dae::PumpComponent::LateUpdate()
{
}

void dae::PumpComponent::Init()
{
}

bool dae::PumpComponent::Deserialize(const std::map<std::string, std::string>&, std::string&)
{
	return true;
}

void dae::PumpComponent::FinishPump()
{
	if (m_IsFinished)
	{
		return;
	}

	m_IsFinished = true;

	if (m_pTailObject != nullptr)
	{
		m_pTailObject->MarkForDeletion();
	}

	if (GetParent() != nullptr)
	{
		GetParent()->MarkForDeletion();
	}

	if (m_pController != nullptr)
	{
		m_pController->OnPlayerEndAttack();
	}
}