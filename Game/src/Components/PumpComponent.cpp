#include <Components/PumpComponent.h>

#include <ComponentFactoryRegistry.h>
#include <Hash.h>
#include <GameObject.h>
#include <Timing.h>

#include <algorithm>
#include <cmath>

dae::PumpComponent::PumpComponent(GameObject* Parent) : Component(Parent)
{
}

void dae::PumpComponent::Configure(glm::vec2 direction, float travelDistance, float speed, std::function<void()> onFinished)
{
	m_Direction = glm::length(direction) > 0.0f ? glm::normalize(direction) : glm::vec2{ 1.0f, 0.0f };
	m_TravelDistance = travelDistance;
	m_Speed = speed;
	m_TravelledDistance = 0.0f;
	m_OnFinished = std::move(onFinished);
	m_IsConfigured = true;
}

void dae::PumpComponent::Update()
{
	if (!m_IsConfigured || m_IsFinished)
	{
		return;
	}

	auto* parent = GetParent();
	if (parent == nullptr)
	{
		FinishPump();
		return;
	}

	auto currentPosition = parent->GetLocalPosition();
	currentPosition += m_Direction * m_Speed * Timing::GetInstance().GetDeltaTime();
	parent->SetLocalPosition(currentPosition);
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
	return false; //this component is meant to be configured at runtime, so we don't support deserialization
}

void dae::PumpComponent::FinishPump()
{
	if (m_IsFinished)
	{
		return;
	}

	m_IsFinished = true;

	if (auto* parent = GetParent())
	{
		parent->MarkForDeletion();
	}

	if (m_OnFinished)
	{
		m_OnFinished();
	}
}