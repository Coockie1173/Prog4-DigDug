#include <Components/PlayerStates/PlayerMove.h>
#include <Components/SwappableRenderComponent.h>
#include <Components/ObjectMoveComponent.h>
#include <Components/PlayerControllerComponent.h>
#include <GameObject.h>
#include <Components/PlayerStates/PlayerIdle.h>
#include <ResourceManager.h>
#include <Timing.h>
#include <Config.h>
#include <Components/TerrainGridComponent.h>
#include <cmath>

namespace dae
{
	const std::string PLAYER_IDLE{ "player/player_idle.png" };
	const std::string PLAYER_WALK{ "player/player_walk.png" };
	constexpr float TIMEANIMS = 0.1f;
	constexpr float SNAP_SPEED_FACTOR = 8.0f;
	constexpr float SNAP_THRESHOLD = 1.0f;

	void PlayerMove::Enter(PlayerControllerComponent& Player)
	{
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

		if (m_pMoveComponent == nullptr)
		{
			m_pMoveComponent = Player.GetPlayer()->GetComponent<ObjectMoveComponent>();
		}

		m_pRenderComponent->SetTexture(m_pIdleTexture);
		WalkTimer = 0.f;
	}

	PlayerState* PlayerMove::Update(PlayerControllerComponent& Player)
	{
		if (Player.GetMoveIntent() == glm::vec2{})
		{
			return m_pStatePool->Get<PlayerIdle>();
		}

		WalkTimer += Timing::GetInstance().GetDeltaTime();
		if (WalkTimer >= TIMEANIMS)
		{
			m_pRenderComponent->SetTexture(m_pRenderComponent->GetTexture() == m_pIdleTexture ? m_pWalkTexture : m_pIdleTexture);
			WalkTimer = 0.f;
		}

		glm::vec2 moveDirection = Player.GetMoveIntent();
		if (glm::length(moveDirection) > 1.0f)
		{
			moveDirection = glm::normalize(moveDirection);
		}

		auto* pPlayerObj = Player.GetPlayer();
		const glm::vec2 pos = pPlayerObj->GetWorldPosition();
		auto* pTerrain = Player.GetTerrainGrid();

		if (pTerrain != nullptr)
		{
			const auto cell = pTerrain->WorldToCell(pos);

			if (moveDirection.x != 0.0f)
			{
				const float snapY = pTerrain->CellToWorldCenter(cell).y;
				const float diff = snapY - pos.y;

				if (std::abs(diff) <= SNAP_THRESHOLD)
				{
					pPlayerObj->SetLocalPosition({ pos.x, pos.y + diff });
				}
				else
				{
					pPlayerObj->SetLocalPosition({ pos.x, pos.y + diff * SNAP_SPEED_FACTOR * Timing::GetInstance().GetDeltaTime() });
				}
			}
			else if (moveDirection.y != 0.0f)
			{
				const float snapX = pTerrain->CellToWorldCenter(cell).x;
				const float diff = snapX - pos.x;

				if (std::abs(diff) <= SNAP_THRESHOLD)
				{
					pPlayerObj->SetLocalPosition({ pos.x + diff, pos.y });
				}
				else
				{
					pPlayerObj->SetLocalPosition({ pos.x + diff * SNAP_SPEED_FACTOR * Timing::GetInstance().GetDeltaTime(), pos.y });
				}
			}
		}

		if (glm::dot(moveDirection, moveDirection) > 0.0f)
		{
			m_pMoveComponent->MoveObject(moveDirection, PLAYER_MOVE_SPEED);
		}

		return nullptr;
	}

	void PlayerMove::Exit(PlayerControllerComponent&)
	{

	}
};