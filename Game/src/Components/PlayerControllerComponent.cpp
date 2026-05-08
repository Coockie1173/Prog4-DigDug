#include <Components/PlayerControllerComponent.h>
#include <GameObject.h>
#include <vector>
#include <ranges>
#include <InputManager.h>
#include <Timing.h>
#include <ResourceManager.h>
#include <SwappableRenderComponent.h>
#include <Components/ObjectMoveComponent.h>
#include <Commands/FrameCounterCommand.h>
#include <Commands/AttackCommand.h>
#include <Commands/MoveIntentCommand.h>
#include <SoundSerivceLocator.h>
#include <glm/geometric.hpp>

#include <Components/PlayerStates/PlayerState.h>
#include <Components/PlayerStates/PlayerIdle.h>

constexpr float TimebetweenFrames = 0.1f;

dae::PlayerControllerComponent::PlayerControllerComponent(dae::GameObject* Parent)
	: Component(Parent), m_pStatePool(std::make_unique<PlayerStatePool>())
{
}

dae::PlayerControllerComponent::~PlayerControllerComponent() = default;

void dae::PlayerControllerComponent::Update()
{
	if (m_pCurrentState)
	{
		if (auto nextState = m_pCurrentState->Update(*this))
		{
			m_pCurrentState->Exit(*this);
			nextState->Enter(*this);
			m_pCurrentState = nextState;
		}
	}

	if (m_WalkTimer > TimebetweenFrames)
	{
		m_WalkTimer = 0;

		//swap out the walking frame
		//m_pRenderComponent->SetTexture(m_pRenderComponent->GetTexture() == m_IdleFrame ? m_WalkFrame : m_IdleFrame);
	}

	/*if (!m_PlayerAttacking)
	{
		glm::vec2 moveDirection = m_MoveIntent;
		if (glm::length(moveDirection) > 1.0f)
		{
			moveDirection = glm::normalize(moveDirection);
		}

		if (glm::dot(moveDirection, moveDirection) > 0.0f)
		{
			m_pMoveComponent->MoveObject(moveDirection, PLAYER_MOVE_SPEED);
			OnPlayerMove();
		}
	}*/

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

	for (auto&& part : m_inputScheme | std::views::split('|')) {
		parts.emplace_back(part.begin(), part.end());
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

	/*
	m_IdleFrame = ResourceManager::GetInstance().LoadTexture(m_IdleFrameName);
	m_WalkFrame = ResourceManager::GetInstance().LoadTexture(m_WalkFrameName);
	m_AttackFrame = ResourceManager::GetInstance().LoadTexture(m_AttackFrameName);

	auto CommandMove = std::make_shared<dae::FrameCounterCommand>(this);
	for (auto& act : parts)
	{
		dae::InputManager::GetInstance().BindActionToCommand(act, CommandMove, dae::InputManager::InputType::Down);
	}
	m_Commands.push_back(std::move(CommandMove));
	m_pRenderComponent->SetTexture(m_IdleFrame);

	auto AttackCommand = std::make_shared<dae::AttackCommand>(this, true);
	auto EndAttackCommand = std::make_shared<dae::AttackCommand>(this, false);
	dae::InputManager::GetInstance().BindActionToCommand(m_attackActionName, AttackCommand, dae::InputManager::InputType::Pressed);
	dae::InputManager::GetInstance().BindActionToCommand(m_attackActionName, EndAttackCommand, dae::InputManager::InputType::Released);
	m_Commands.push_back(std::move(AttackCommand));
	m_Commands.push_back(std::move(EndAttackCommand));*/
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
	m_PlayerAttacking = true;
}

void dae::PlayerControllerComponent::OnPlayerEndAttack()
{
	m_PlayerAttacking = false;
}

void dae::PlayerControllerComponent::AddMoveIntent(const glm::vec2& direction)
{
	m_MoveIntent += direction;
}