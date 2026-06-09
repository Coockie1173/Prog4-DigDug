#include <Components/PlayerStates/PlayerIdle.h>
#include <Components/SwappableRenderComponent.h>
#include <Components/PlayerControllerComponent.h>
#include <GameObject.h>
#include <Components/PlayerStates/PlayerMove.h>
#include <Components/PlayerStates/PlayerDig.h>
#include <ResourceManager.h>
#include <Components/TerrainGridComponent.h>

namespace dae
{
	constexpr float SNAP_THRESHOLD = 1.0f;
	const std::string PLAYER_IDLE{ "player/player_idle.png" };

	void PlayerIdle::Enter(PlayerControllerComponent& Player)
	{
		if (m_pRenderComponent == nullptr)
		{
			m_pRenderComponent = Player.GetPlayer()->GetComponent<SwappableRenderComponent>();
		}

		if (m_pIdleTexture == nullptr)
		{
			m_pIdleTexture = ResourceManager::GetInstance().LoadTexture(PLAYER_IDLE);
		}
		m_pRenderComponent->SetTexture(m_pIdleTexture);
	}

	PlayerState* PlayerIdle::Update(PlayerControllerComponent& Player)
	{
		if (Player.GetMoveIntent() == glm::vec2{})
		{
			return nullptr;
		}

		auto* terrain = Player.GetTerrainGrid();
		const glm::vec2 pos = Player.GetPlayer()->GetWorldPosition();
		const glm::vec2 intent = Player.GetMoveIntent();

		if (terrain != nullptr)
		{
			const glm::vec2 cellCenter = terrain->CellToWorldCenter(terrain->WorldToCell(pos));
			const float perpendicularDiff = (intent.x != 0.0f)
				? std::abs(cellCenter.y - pos.y)
				: std::abs(cellCenter.x - pos.x);

			if (perpendicularDiff <= SNAP_THRESHOLD)
			{
				const auto currentCell = terrain->WorldToCell(pos);
				const glm::ivec2 nextCell = currentCell + glm::ivec2(glm::round(intent));
				if (terrain->IsValidCell(nextCell) && terrain->IsSolidCell(nextCell))
				{
					return m_pStatePool->Get<PlayerDig>();
				}
			}
		}

		return m_pStatePool->Get<PlayerMove>();
	}

	void PlayerIdle::Exit(PlayerControllerComponent&)
	{
		
	}
};
