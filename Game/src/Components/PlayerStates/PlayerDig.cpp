#include <Components/PlayerStates/PlayerDig.h>
#include <Components/PlayerControllerComponent.h>
#include <Components/SwappableRenderComponent.h>
#include <Components/ObjectMoveComponent.h>
#include <Components/TerrainGridComponent.h>
#include <GameObject.h>
#include <Components/PlayerStates/PlayerIdle.h>
#include <Components/PlayerStates/PlayerMove.h>
#include <ResourceManager.h>
#include <Timing.h>
#include <algorithm>
#include <Config.h>

namespace dae
{
	const std::string PLAYER_DIG{ "player/player_dig.png" };

	void PlayerDig::Enter(PlayerControllerComponent& Player)
	{
		if (m_pRenderComponent == nullptr)
		{
			m_pRenderComponent = Player.GetPlayer()->GetComponent<SwappableRenderComponent>();
		}
		if (m_pMoveComponent == nullptr)
		{
			m_pMoveComponent = Player.GetPlayer()->GetComponent<ObjectMoveComponent>();
		}
		m_pTerrainGrid = Player.GetTerrainGrid();

		if (m_pDigTexture == nullptr)
		{
			m_pDigTexture = ResourceManager::GetInstance().LoadTexture(PLAYER_DIG);
		}
		if (m_pRenderComponent != nullptr)
		{
			m_pRenderComponent->SetTexture(m_pDigTexture);
		}

		m_DigTimer = 0.0f;
		if (m_pTerrainGrid != nullptr)
		{
			// Use half the player size as radius so all overlapping cells get carved
			const auto pos = Player.GetPlayer()->GetWorldPosition();
			m_pTerrainGrid->CarveAtWorldPosition({ pos.x, pos.y }, PLAYERHALFSIZE);
		}
		//Player.OnPlayerDig();
	}

	PlayerState* PlayerDig::Update(PlayerControllerComponent& Player)
	{
		if (!Player.GetIsDigging())
		{
			if (Player.GetMoveIntent() != glm::vec2{})
			{
				return m_pStatePool->Get<PlayerMove>();
			}
			return m_pStatePool->Get<PlayerIdle>();
		}

		if (!Player.GetIsDigging())
		{
			if (Player.GetMoveIntent() != glm::vec2{})
			{
				return m_pStatePool->Get<PlayerMove>();
			}
			return m_pStatePool->Get<PlayerIdle>();
		}

		if (m_pTerrainGrid != nullptr)
		{
			const auto pos = Player.GetPlayer()->GetWorldPosition();
			m_pTerrainGrid->CarveAtWorldPosition({ pos.x, pos.y }, PLAYERHALFSIZE);
		}

		Player.OnPlayerEndDig();
		if (Player.GetMoveIntent() != glm::vec2{})
		{
			return m_pStatePool->Get<PlayerMove>();
		}
		return m_pStatePool->Get<PlayerIdle>();
	}

	void PlayerDig::Exit(PlayerControllerComponent& Player)
	{
		Player.OnPlayerEndDig();
		Player.ClearMoveIntent();
	}
}
