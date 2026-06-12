#include <algorithm>
#include <cmath>
#include <Commands/AttackCommand.h>
#include <Commands/MoveIntentCommand.h>
#include <ComponentFactoryRegistry.h>
#include <Components/ObjectMoveComponent.h>
#include <Components/PlayerControllerComponent.h>
#include <Components/EnemyComponent.h>
#include <Components/PlayerStates/PlayerAttack.h>
#include <Components/PlayerStates/PlayerDig.h>
#include <Components/PlayerStates/PlayerIdle.h>
#include <Components/PlayerStates/PlayerPumping.h>
#include <Components/PlayerStates/PlayerStart.h>
#include <Components/PlayerStates/PlayerDead.h>
#include <Components/PlayerStates/PlayerState.h>
#include <Components/TerrainGridComponent.h>
#include <GameObject.h>
#include <EventManager.h>
#include <Hash.h>
#include <InputManager.h>
#include <ranges>
#include <Scene.h>
#include <string_view>
#include <SwappableRenderComponent.h>
#include <Timing.h>
#include <vector>

namespace
{
	const bool PlayerControllerComponentRegistered = dae::RegisterComponentFactoryFor<dae::PlayerControllerComponent>(make_sdbm_hash("PlayerControllerComponent"));

	constexpr float ATTACK_COOLDOWN = 0.25f;

	// Splits a string by a delimiter and returns a vector of string_view parts.
	// The returned views are valid as long as the source string lives.
	std::vector<std::string_view> SplitStringView(std::string_view source, char delimiter)
	{
		std::vector<std::string_view> parts;
		for (const auto& part : source | std::views::split(delimiter))
		{
			parts.emplace_back(part.begin(), part.end());
		}
		return parts;
	}
}

dae::PlayerControllerComponent::PlayerControllerComponent(dae::GameObject* Parent)
	: Component(Parent), m_pStatePool(std::make_unique<StatePool<PlayerState>>())
{}

dae::PlayerControllerComponent::~PlayerControllerComponent() = default;

void dae::PlayerControllerComponent::Update()
{
	const float dt = Timing::GetInstance().GetDeltaTime();

	m_AttackCooldownRemaining = std::max(0.0f, m_AttackCooldownRemaining - dt);
	m_DigCooldownRemaining = std::max(0.0f, m_DigCooldownRemaining - dt);

	m_MoveIntent = ResolveCardinalMoveIntent();
	UpdateFacingFromMoveIntent(m_MoveIntent);

	if (m_pCurrentState)
	{
		if (auto* nextState = m_pCurrentState->Update(*this))
		{
			m_pCurrentState->Exit(*this);
			nextState->Enter(*this);
			m_pCurrentState = nextState;
		}
	}

	if (m_PlayerDied)
	{
		return;
	}

	auto* terrain = GetTerrainGrid();

	if (terrain != nullptr)
	{
		const auto currentCell = terrain->WorldToCell(GetPlayer()->GetWorldPosition());

		if (m_HasPreviousCell && currentCell != m_PreviousCell && !m_PlayerDigging)
		{
			terrain->CarveConnection(m_PreviousCell, currentCell);
		}

		m_PreviousCell = currentCell;
		m_HasPreviousCell = true;
	}

	auto* scene = GetPlayer()->GetScene();
	if (scene != nullptr)
	{
		scene->ForEachGameObject([&](GameObject* obj)
			{
				if (obj == GetParent()) return;
				auto* enemy = obj->GetComponent<EnemyComponent>();
				if (enemy == nullptr) return;

				if (GetParent()->OverlapsWith(obj))
				{
					PlayerDeath();
				}
			});
	}

	ApplyFacingToRenderComponent();

	ClearMoveIntent();
}

void dae::PlayerControllerComponent::LateUpdate()
{}

void dae::PlayerControllerComponent::Init()
{
	m_pRenderComponent = GetParent()->GetComponent<SwappableRenderComponent>();
	m_pMoveComponent = GetParent()->GetComponent<ObjectMoveComponent>();

	if (m_pRenderComponent == nullptr || m_pMoveComponent == nullptr)
	{
		return;
	}

	m_pRenderComponent->SetPriority(true);

	//now generate all input actions
	auto parts = SplitStringView(m_inputScheme, '|');
	if (parts.size() < 2)
	{
		//turns out using the debugger DURING INIT fucks up the entire flow of the program, who could've thunk it??
		//Debugger::GetInstance().LogError("PlayerControllerComponent inputScheme should have 4 parts separated by '|', but got " + std::to_string(parts.size()));
		return;
	}
	//Debugger::GetInstance().LogDebug("PlayerControllerComponent input scheme set to: up='" + parts[0] + "', down='" + parts[1] + "', left='" + parts[2] + "', right='" + parts[3] + "'");

	auto bindAxis = [&](std::string_view actionName, glm::vec2 axis)
		{
			auto cmd = std::make_shared<dae::MoveIntentCommand>(this, axis);
			dae::InputManager::GetInstance().BindAxisActionToCommand(std::string(actionName), cmd);
			m_Commands.push_back(std::move(cmd));
		};

	auto bindAction = [&](std::string_view actionName, bool pressed)
		{
			auto cmd = std::make_shared<dae::AttackCommand>(this, pressed);
			const auto inputType = pressed ? dae::InputManager::InputType::Pressed
				: dae::InputManager::InputType::Released;
			dae::InputManager::GetInstance().BindActionToCommand(std::string(actionName), cmd, inputType);
			m_Commands.push_back(std::move(cmd));
		};


	//if we have four it means we also bound controller/keyboard to this player
	if (parts.size() > 2)
	{
		bindAxis(parts[0], { -1.f,  0.f });
		bindAxis(parts[1], { 0.f, -1.f });
		bindAxis(parts[2], { 1.f,  0.f });
		bindAxis(parts[3], { 0.f, -1.f });
		m_PrimaryPlayer = true; //player one has four bindings
	}
	else
	{
		bindAxis(parts[0], { 1.f,  0.f });
		bindAxis(parts[1], { 0.f, -1.f });
	}

	parts = SplitStringView(m_attackActionName, '|');

	bindAction(parts[0], true);
	bindAction(parts[0], false);
	if (parts.size() > 1)
	{
		bindAction(parts[1], true);
		bindAction(parts[1], false);
	}

	m_pCurrentState = m_pStatePool->Get<PlayerStart>();
	m_pCurrentState->Enter(*this);
	m_PlayerDigging = false;
	m_DigCooldownRemaining = 0.0f;

}

bool dae::PlayerControllerComponent::Deserialize(const std::map<std::string, std::string>& properties, std::string& errorMessage)
{
	if (!GetRequiredProperty(properties, "inputScheme", m_inputScheme, errorMessage, "PlayerControllerComponent")) return false;
	if (!GetRequiredProperty(properties, "attackActionName", m_attackActionName, errorMessage, "PlayerControllerComponent")) return false;
	if (!GetRequiredProperty(properties, "cancelActionName", m_cancelActionName, errorMessage, "PlayerControllerComponent")) return false;

	return true;
}

void dae::PlayerControllerComponent::OnPlayerMove()
{
	if (m_PlayerAttacking || m_PlayerDied)
		return;
	m_WalkTimer += Timing::GetInstance().GetDeltaTime();
}

void dae::PlayerControllerComponent::OnPlayerAttack()
{
	if (m_PlayerDied)
	{
		return;
	}

	if (m_pStatePool->Get<PlayerStart>() == m_pCurrentState)
	{
		//don't do anything in the start state
		return;
	}
	if (m_pStatePool->Get<PlayerPumping>() == m_pCurrentState)
	{
		auto* p = dynamic_cast<PlayerPumping*>(m_pCurrentState);
		p->Pump();
		return;
	}

	if (m_PlayerAttacking || m_AttackCooldownRemaining > 0.0f)
	{
		return;
	}

	m_PlayerAttacking = true;

	if (m_pCurrentState == nullptr)
	{
		return;
	}

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
	m_AttackCooldownRemaining = ATTACK_COOLDOWN;
}

void dae::PlayerControllerComponent::OnPlayerEndDig()
{
	m_PlayerDigging = false;
}

void dae::PlayerControllerComponent::OnPlayerDig()
{
	if (m_pStatePool->Get<PlayerStart>() == m_pCurrentState)
	{
		//don't do anything in the start state
		return;
	}

	auto* terrain = GetTerrainGrid();
	if (terrain == nullptr || !terrain->HasSolidAtWorldPosition(GetPlayer()->GetWorldPosition()))
	{
		m_PlayerDigging = false;
		return;
	}

	m_PlayerDigging = true;
	if (m_pCurrentState == nullptr)
	{
		return;
	}

	m_PlayerDigging = true;

	auto* pDigState = m_pStatePool->Get<PlayerDig>();
	if (m_pCurrentState != pDigState)
	{
		m_pCurrentState->Exit(*this);
		pDigState->Enter(*this);
		m_pCurrentState = pDigState;
	}
}

dae::TerrainGridComponent* dae::PlayerControllerComponent::GetTerrainGrid() const
{
	if (m_pCachedTerrainGrid != nullptr)
		return m_pCachedTerrainGrid;

	auto* player = GetPlayer();
	if (player == nullptr)
		return nullptr;

	auto* scene = player->GetScene();
	if (scene == nullptr)
		return nullptr;

	auto* terrainObject = scene->FindGameObject("Terrain");
	if (terrainObject == nullptr)
		return nullptr;

	m_pCachedTerrainGrid = terrainObject->GetComponent<dae::TerrainGridComponent>();
	return m_pCachedTerrainGrid;
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

	// When both axes are active, pick the axis of the most recent input.
	// Tie-break in favour of X.
	const bool preferX = m_MoveIntent.x != 0.0f && (m_MoveIntent.y == 0.0f || std::abs(m_LastMoveIntent.x) >= std::abs(m_LastMoveIntent.y));

	if (preferX)
	{
		return { m_MoveIntent.x > 0.0f ? 1.0f : -1.0f, 0.0f };
	}

	return { 0.0f, m_MoveIntent.y > 0.0f ? 1.0f : -1.0f };
}

void dae::PlayerControllerComponent::UpdateFacingFromMoveIntent(const glm::vec2& moveIntent)
{
	if (m_pStatePool->Get<PlayerStart>() == m_pCurrentState)
	{
		//don't do anything in the start state
		return;
	}
	if (m_pStatePool->Get<PlayerAttack>() == m_pCurrentState)
	{
		//don't do anything in the start state
		return;
	}
	if (m_pStatePool->Get<PlayerPumping>() == m_pCurrentState)
	{
		//don't do anything in the start state
		return;
	}

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

	m_pRenderComponent->SetFacingDirection(GetFacingVector());
}

void dae::PlayerControllerComponent::OnPlayerHitEnemy(EnemyComponent* enemy)
{
	auto* pumpingState = m_pStatePool->Get<PlayerPumping>();
	pumpingState->SetTarget(enemy);

	m_pCurrentState->Exit(*this);
	pumpingState->Enter(*this);
	m_pCurrentState = pumpingState;
}

void dae::PlayerControllerComponent::AlertPumperDone()
{
	auto* IdleState = m_pStatePool->Get<PlayerIdle>();

	m_pCurrentState->Exit(*this);
	IdleState->Enter(*this);
	m_pCurrentState = IdleState;
}

void dae::PlayerControllerComponent::PlayerDeath()
{
	m_PlayerDied = true;

	auto* DeadState = m_pStatePool->Get<PlayerDead>();
	m_pCurrentState->Exit(*this);
	DeadState->Enter(*this);
	m_pCurrentState = DeadState;
	//EventManager::GetInstance().Publish(PlayerControllerComponent::PLAYERDEADHASH);
}