#include <Components/PlayerStates/PlayerStart.h>
#include <Components/PlayerStates/PlayerIdle.h>
#include <Config.h>
#include <Components/PlayerControllerComponent.h>
#include <Components/ObjectMoveComponent.h>
#include <Components/TerrainGridComponent.h>
#include <Components/SwappableRenderComponent.h>
#include <GameObject.h>
#include <Component.h>
#include <glm/vec2.hpp>
#include <Timing.h>
#include <ResourceManager.h>

namespace dae
{
	namespace
	{
		const std::string PLAYER_IDLE{ "player/player_idle.png" };
		const std::string PLAYER_WALK{ "player/player_walk.png" };
		constexpr float TIMEANIMS = 0.1f;
	}

	void PlayerStart::Enter(PlayerControllerComponent& Player)
	{
		if (m_pMoveComponent == nullptr)
		{
			m_pMoveComponent = Player.GetPlayer()->GetComponent<ObjectMoveComponent>();
		}

		if (m_pRenderComponent == nullptr)
		{
			m_pRenderComponent = Player.GetPlayer()->GetComponent<SwappableRenderComponent>();
		}

		if (m_pIdleTexture == nullptr)
		{
			m_pIdleTexture = ResourceManager::GetInstance().LoadTexture(PLAYER_IDLE);
		}
		if (m_pWalkTexture == nullptr)
		{
			m_pWalkTexture = ResourceManager::GetInstance().LoadTexture(PLAYER_WALK);
		}

		m_pRenderComponent->SetTexture(m_pIdleTexture);

		Player.GetParent()->SetPosition(glm::vec2(0, 20));
		m_pTerrainGrid = Player.GetTerrainGrid();
		if (m_pTerrainGrid != nullptr)
		{
			const auto cell = m_pTerrainGrid->WorldToCell(Player.GetPlayer()->GetWorldPosition());
			m_pTerrainGrid->CarveCell(cell);
		}
	}

	PlayerState* PlayerStart::Update(PlayerControllerComponent& Player)
	{
		//move to center, then move down
		//I know it should be two states but it's a lil silly surely
		//considering the player doesn't "auto move" besides on the spawn state

		WalkTimer += Timing::GetInstance().GetDeltaTime();
		if (WalkTimer >= TIMEANIMS)
		{
			m_pRenderComponent->SetTexture(m_pRenderComponent->GetTexture() == m_pIdleTexture ? m_pWalkTexture : m_pIdleTexture);
			WalkTimer = 0.f;
		}

		if (m_pTerrainGrid != nullptr)
		{
			const auto cell = m_pTerrainGrid->WorldToCell(Player.GetPlayer()->GetWorldPosition());
			if (m_pTerrainGrid->IsSolidCell(cell))
			{
				m_pTerrainGrid->CarveCell(cell);
			}
		}
		
		glm::vec2 tmp{ 0,0 };
		if (Player.GetParent()->GetWorldPosition().x < SCREENWIDTH / RENDERSCALE / 2 + 8)
		{
			tmp = { 1,0 };
		}
		else if (Player.GetParent()->GetWorldPosition().y < SCREENHEIGHT / RENDERSCALE / 2)
		{
			tmp = { 0,1 };
		}
		else
		{
			return m_pStatePool->Get<PlayerIdle>();
		}
		m_pMoveComponent->MoveObject(tmp, PLAYER_MOVE_SPEED);

		return nullptr;
	}

	void PlayerStart::Exit(PlayerControllerComponent&)
	{

	}
};