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
	dae::InputManager::GetInstance().BindActionToCommand(parts[0], CommandUp);
	m_Commands.push_back(std::move(CommandUp));

	auto CommandDown = std::make_shared<dae::MovementInputCommand>(m_pMoveComponent);
	CommandDown->SetDirection(glm::vec2(0, 1));
	CommandDown->SetSpeed(PLAYER_MOVE_SPEED);
	dae::InputManager::GetInstance().BindActionToCommand(parts[1], CommandDown);
	m_Commands.push_back(std::move(CommandDown));

	auto CommandLeft = std::make_shared<dae::MovementInputCommand>(m_pMoveComponent);
	CommandLeft->SetDirection(glm::vec2(-1, 0));
	CommandLeft->SetSpeed(PLAYER_MOVE_SPEED);
	dae::InputManager::GetInstance().BindActionToCommand(parts[2], CommandLeft);
	m_Commands.push_back(std::move(CommandLeft));

	auto CommandRight = std::make_shared<dae::MovementInputCommand>(m_pMoveComponent);
	CommandRight->SetDirection(glm::vec2(1, 0));
	CommandRight->SetSpeed(PLAYER_MOVE_SPEED);
	dae::InputManager::GetInstance().BindActionToCommand(parts[3], CommandRight);
	m_Commands.push_back(std::move(CommandRight));

	m_IdleFrame = ResourceManager::GetInstance().LoadTexture(m_IdleFrameName);
	m_WalkFrame = ResourceManager::GetInstance().LoadTexture(m_WalkFrameName);
	m_AttackFrame = ResourceManager::GetInstance().LoadTexture(m_AttackFrameName);

	auto CommandMove = std::make_shared<dae::FrameCounterCommand>(this);
	for (auto& act : parts)
	{
		dae::InputManager::GetInstance().BindActionToCommand(act, CommandMove);
	}
	m_Commands.push_back(std::move(CommandMove));
	m_pRenderComponent->SetTexture(m_IdleFrame);
}

bool dae::PlayerControllerComponent::Deserialize(const std::map<std::string, std::string>& properties, std::string& errorMessage)
{
	auto inputSchemeIt = properties.find("inputScheme");
	auto idleFrameIt = properties.find("IdleFrameName");
	auto walkFrameIt = properties.find("WalkFrameName");
	auto attackFrameIt = properties.find("AttackFrameName");
	if (inputSchemeIt != properties.end())
	{
		m_inputScheme = inputSchemeIt->second;
	}
	if (idleFrameIt != properties.end())
	{
		m_IdleFrameName = idleFrameIt->second;
	}

	if (walkFrameIt != properties.end())
	{
		m_WalkFrameName = walkFrameIt->second;
	}

	if (attackFrameIt != properties.end())
	{
		m_AttackFrameName = attackFrameIt->second;
		return true;
	}

	errorMessage = "PlayerControllerComponent requires 'inputScheme' property.";
	return false;
}

void dae::PlayerControllerComponent::OnPlayerMove()
{
	m_WalkTimer += Timing::GetInstance().GetDeltaTime();
}