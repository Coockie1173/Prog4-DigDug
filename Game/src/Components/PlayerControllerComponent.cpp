#include <Components/PlayerControllerComponent.h>
#include <GameObject.h>
#include <vector>
#include <ranges>
#include <MovementInputCommand.h>
#include <InputManager.h>
#include <Timing.h>
#include <ResourceManager.h>
#include <SwappableRenderComponent.h>
#include <Commands/FrameCounterCommand.h>
#include <Commands/AttackCommand.h>
#include <SoundSerivceLocator.h>

constexpr float PLAYER_MOVE_SPEED = 100.0f;
constexpr float TimebetweenFrames = 0.1f;

dae::PlayerControllerComponent::PlayerControllerComponent(dae::GameObject* Parent) : Component(Parent)
{
}

void dae::PlayerControllerComponent::Update()
{
	if (m_WalkTimer > TimebetweenFrames)
	{
		m_WalkTimer = 0;

		//swap out the walking frame
		m_pRenderComponent->SetTexture(m_pRenderComponent->GetTexture() == m_IdleFrame ? m_WalkFrame : m_IdleFrame);
	}
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

	auto CommandUp = std::make_shared<dae::MovementInputCommand>(m_pMoveComponent);
	CommandUp->SetDirection(glm::vec2(0, -1));
	CommandUp->SetSpeed(PLAYER_MOVE_SPEED);
	dae::InputManager::GetInstance().BindActionToCommand(parts[0], CommandUp, dae::InputManager::InputType::Down);
	m_Commands.push_back(std::move(CommandUp));

	auto CommandDown = std::make_shared<dae::MovementInputCommand>(m_pMoveComponent);
	CommandDown->SetDirection(glm::vec2(0, 1));
	CommandDown->SetSpeed(PLAYER_MOVE_SPEED);
	dae::InputManager::GetInstance().BindActionToCommand(parts[1], CommandDown, dae::InputManager::InputType::Down);
	m_Commands.push_back(std::move(CommandDown));

	auto CommandLeft = std::make_shared<dae::MovementInputCommand>(m_pMoveComponent);
	CommandLeft->SetDirection(glm::vec2(-1, 0));
	CommandLeft->SetSpeed(PLAYER_MOVE_SPEED);
	dae::InputManager::GetInstance().BindActionToCommand(parts[2], CommandLeft, dae::InputManager::InputType::Down);
	m_Commands.push_back(std::move(CommandLeft));

	auto CommandRight = std::make_shared<dae::MovementInputCommand>(m_pMoveComponent);
	CommandRight->SetDirection(glm::vec2(1, 0));
	CommandRight->SetSpeed(PLAYER_MOVE_SPEED);
	dae::InputManager::GetInstance().BindActionToCommand(parts[3], CommandRight, dae::InputManager::InputType::Down);
	m_Commands.push_back(std::move(CommandRight));

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
	m_Commands.push_back(std::move(EndAttackCommand));
}

bool dae::PlayerControllerComponent::Deserialize(const std::map<std::string, std::string>& properties, std::string& errorMessage)
{
	//I'll need to apply this pattern in other places too lateron
	//TODO: port pattern to other components
	auto getRequired = [&](const std::string& key, std::string& target) -> bool
		{
			auto it = properties.find(key);
			if (it == properties.end())
			{
				errorMessage = "PlayerControllerComponent requires '" + key + "' property.";
				return false;
			}

			target = it->second;
			return true;
		};

	if (!getRequired("inputScheme", m_inputScheme)) return false;
	if (!getRequired("IdleFrameName", m_IdleFrameName)) return false;
	if (!getRequired("WalkFrameName", m_WalkFrameName)) return false;
	if (!getRequired("AttackFrameName", m_AttackFrameName)) return false;
	if (!getRequired("attackActionName", m_attackActionName)) return false;

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
	m_pMoveComponent->SetIsDisabled(true);
	m_pRenderComponent->SetTexture(m_AttackFrame);

	SoundServiceLocator::GetSoundSystem().PlaySound("Data/sound/Shoot.wav");
}

void dae::PlayerControllerComponent::OnPlayerEndAttack()
{
	m_PlayerAttacking = false;
	m_pMoveComponent->SetIsDisabled(false);
	m_pRenderComponent->SetTexture(m_IdleFrame);
}