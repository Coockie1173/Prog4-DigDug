#include <Components/PlayerControllerComponent.h>
#include <ComponentFactoryRegistry.h>
#include <Hash.h>
#include <GameObject.h>
#include <vector>
#include <InputManager.h>
#include <Timing.h>
#include <SwappableRenderComponent.h>
#include <Components/ObjectMoveComponent.h>
#include <Commands/AttackCommand.h>
#include <Commands/MoveIntentCommand.h>

#include <cmath>
#include <algorithm>

#include <Components/PlayerStates/PlayerState.h>
#include <Components/PlayerStates/PlayerIdle.h>
#include <Components/PlayerStates/PlayerAttack.h>

namespace
{
	 const bool PlayerControllerComponentRegistered = dae::RegisterComponentFactoryFor<dae::PlayerControllerComponent>(make_sdbm_hash("PlayerControllerComponent"));
}

dae::PlayerControllerComponent::PlayerControllerComponent(dae::GameObject* Parent)
	: Component(Parent), m_pStatePool(std::make_unique<PlayerStatePool>())
{
}

dae::PlayerControllerComponent::~PlayerControllerComponent() = default;

void dae::PlayerControllerComponent::Update()
{
	if (m_AttackCooldownRemaining > 0.0f)
	{
		m_AttackCooldownRemaining = std::max(0.0f, m_AttackCooldownRemaining - Timing::GetInstance().GetDeltaTime());
	}

	m_MoveIntent = ResolveCardinalMoveIntent();
	UpdateFacingFromMoveIntent(m_MoveIntent);

	if (m_pCurrentState)
	{
		if (auto nextState = m_pCurrentState->Update(*this))
		{
			m_pCurrentState->Exit(*this);
			nextState->Enter(*this);
			m_pCurrentState = nextState;
		}
	}

	ApplyFacingToRenderComponent();

	ClearMoveIntent();
}

void dae::PlayerControllerComponent::LateUpdate()
{
}

void dae::PlayerControllerComponent::Init()
{
	m_pRenderComponent = GetParent()->GetComponent<SwappableRenderComponent>();
	m_pMoveComponent = GetParent()->GetComponent<ObjectMoveComponent>();

	//now generate all input actions
	std::vector<std::string> parts;

	std::size_t startIndex = 0;
	while (startIndex <= m_inputScheme.size())
	{
		const auto separatorIndex = m_inputScheme.find('|', startIndex);
		if (separatorIndex == std::string::npos)
		{
			parts.emplace_back(m_inputScheme.substr(startIndex));
			break;
		}

		parts.emplace_back(m_inputScheme.substr(startIndex, separatorIndex - startIndex));
		startIndex = separatorIndex + 1;
	}
	if (parts.size() != 4)
	{
		//turns out using the debugger DURING INIT fucks up the entire flow of the program, who could've thunk it??
		//Debugger::GetInstance().LogError("PlayerControllerComponent inputScheme should have 4 parts separated by '|', but got " + std::to_string(parts.size()));
		return;
	}
	//Debugger::GetInstance().LogDebug("PlayerControllerComponent input scheme set to: up='" + parts[0] + "', down='" + parts[1] + "', left='" + parts[2] + "', right='" + parts[3] + "'");

	std::shared_ptr<dae::Command> m_MoveUpCommand{};
	std::shared_ptr<dae::Command> m_MoveDownCommand{};
	std::shared_ptr<dae::Command> m_MoveLeftCommand{};
	std::shared_ptr<dae::Command> m_MoveRightCommand{};
	std::shared_ptr<dae::Command> m_AttackStartCommand{};
	std::shared_ptr<dae::Command> m_AttackEndCommand{};

	m_MoveUpCommand = std::make_shared<dae::MoveIntentCommand>(this, glm::vec2(0, -1));
	dae::InputManager::GetInstance().BindActionToCommand(parts[0], m_MoveUpCommand, dae::InputManager::InputType::Down);
	m_Commands.push_back(std::move(m_MoveUpCommand));

	m_MoveDownCommand = std::make_shared<dae::MoveIntentCommand>(this, glm::vec2(0, 1));
	dae::InputManager::GetInstance().BindActionToCommand(parts[1], m_MoveDownCommand, dae::InputManager::InputType::Down);
	m_Commands.push_back(std::move(m_MoveDownCommand));

	m_MoveLeftCommand = std::make_shared<dae::MoveIntentCommand>(this, glm::vec2(-1, 0));
	dae::InputManager::GetInstance().BindActionToCommand(parts[2], m_MoveLeftCommand, dae::InputManager::InputType::Down);
	m_Commands.push_back(std::move(m_MoveLeftCommand));

	m_MoveRightCommand = std::make_shared<dae::MoveIntentCommand>(this, glm::vec2(1, 0));
	dae::InputManager::GetInstance().BindActionToCommand(parts[3], m_MoveRightCommand, dae::InputManager::InputType::Down);
	m_Commands.push_back(std::move(m_MoveRightCommand));

	m_AttackStartCommand = std::make_shared<dae::AttackCommand>(this, true);
	m_AttackEndCommand = std::make_shared<dae::AttackCommand>(this, false);
	dae::InputManager::GetInstance().BindActionToCommand(m_attackActionName, m_AttackStartCommand, dae::InputManager::InputType::Pressed);
	dae::InputManager::GetInstance().BindActionToCommand(m_attackActionName, m_AttackEndCommand, dae::InputManager::InputType::Released);
	m_Commands.push_back(std::move(m_AttackStartCommand));
	m_Commands.push_back(std::move(m_AttackEndCommand));

	m_pCurrentState = m_pStatePool->Get<PlayerIdle>();
	m_pCurrentState->Enter(*this);

}

bool dae::PlayerControllerComponent::Deserialize(const std::map<std::string, std::string>& properties, std::string& errorMessage)
{
	if (!GetRequiredProperty(properties, "inputScheme", m_inputScheme, errorMessage, "PlayerControllerComponent")) return false;
	if (!GetRequiredProperty(properties, "attackActionName", m_attackActionName, errorMessage, "PlayerControllerComponent")) return false;

	return true;
}

void dae::PlayerControllerComponent::OnPlayerMove()
{
	if (m_PlayerAttacking)
		return;
	m_WalkTimer += Timing::GetInstance().GetDeltaTime();
}

void dae::PlayerControllerComponent::OnPlayerAttack()
{
	if (m_PlayerAttacking || m_AttackCooldownRemaining > 0.0f)
		return;

	m_PlayerAttacking = true;

	if (m_pCurrentState == nullptr)
		return;

	auto* pAttackState = m_pStatePool->Get<PlayerAttack>();
	if (m_pCurrentState != pAttackState)
	{
		m_pCurrentState->Exit(*this);
		pAttackState->Enter(*this);
		m_pCurrentState = pAttackState;
	}
}

void dae::PlayerControllerComponent::OnPlayerEndAttack()
{
	m_PlayerAttacking = false;
	m_AttackCooldownRemaining = 0.25f;
}

glm::vec2 dae::PlayerControllerComponent::GetFacingVector() const
{
	switch (m_FacingDirection)
	{
	case FacingDirection::Right:
		return { 1.0f, 0.0f };
	case FacingDirection::Left:
		return { -1.0f, 0.0f };
	case FacingDirection::Up:
		return { 0.0f, -1.0f };
	case FacingDirection::Down:
		return { 0.0f, 1.0f };
	}

	return { 1.0f, 0.0f };
}

void dae::PlayerControllerComponent::AddMoveIntent(const glm::vec2& direction)
{
	m_MoveIntent += direction;
	m_LastMoveIntent = direction;
}

glm::vec2 dae::PlayerControllerComponent::ResolveCardinalMoveIntent() const
{
	if (m_MoveIntent.x == 0.0f && m_MoveIntent.y == 0.0f)
	{
		return {};
	}

	if (m_MoveIntent.x != 0.0f && m_MoveIntent.y != 0.0f)
	{
		if (std::abs(m_LastMoveIntent.x) >= std::abs(m_LastMoveIntent.y))
		{
			return { m_MoveIntent.x > 0.0f ? 1.0f : -1.0f, 0.0f };
		}

		return { 0.0f, m_MoveIntent.y > 0.0f ? 1.0f : -1.0f };
	}

	if (m_MoveIntent.x != 0.0f)
	{
		return { m_MoveIntent.x > 0.0f ? 1.0f : -1.0f, 0.0f };
	}

	return { 0.0f, m_MoveIntent.y > 0.0f ? 1.0f : -1.0f };
}

void dae::PlayerControllerComponent::UpdateFacingFromMoveIntent(const glm::vec2& moveIntent)
{
	if (moveIntent.x > 0.0f)
	{
		m_FacingDirection = FacingDirection::Right;
	}
	else if (moveIntent.x < 0.0f)
	{
		m_FacingDirection = FacingDirection::Left;
	}
	else if (moveIntent.y < 0.0f)
	{
		m_FacingDirection = FacingDirection::Up;
	}
	else if (moveIntent.y > 0.0f)
	{
		m_FacingDirection = FacingDirection::Down;
	}
}

void dae::PlayerControllerComponent::ApplyFacingToRenderComponent() const
{
	if (m_pRenderComponent == nullptr)
	{
		return;
	}

	switch (m_FacingDirection)
	{
	case FacingDirection::Right:
		m_pRenderComponent->SetFacingDirection({ 1.0f, 0.0f });
		break;
	case FacingDirection::Left:
		m_pRenderComponent->SetFacingDirection({ -1.0f, 0.0f });
		break;
	case FacingDirection::Up:
		m_pRenderComponent->SetFacingDirection({ 0.0f, -1.0f });
		break;
	case FacingDirection::Down:
		m_pRenderComponent->SetFacingDirection({ 0.0f, 1.0f });
		break;
	}
}