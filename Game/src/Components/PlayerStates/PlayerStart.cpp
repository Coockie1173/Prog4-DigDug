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
#include <EventManager.h>

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
            m_pRenderComponent->SetTexture(
                m_pRenderComponent->GetTexture() == m_pIdleTexture
                ? m_pWalkTexture
                : m_pIdleTexture
            );
            WalkTimer = 0.f;
        }

        if (m_pTerrainGrid != nullptr)
        {
            const auto cell = m_pTerrainGrid->WorldToCell(
                Player.GetPlayer()->GetWorldPosition()
            );

            if (m_pTerrainGrid->IsSolidCell(cell))
            {
                m_pTerrainGrid->CarveCell(cell);
            }
        }

        auto* parent = Player.GetParent();
        const glm::vec2 pos = parent->GetWorldPosition();

        const glm::vec2 terrainCenter =
            m_pTerrainGrid
            ? m_pTerrainGrid->CellToWorldCenter({
                m_pTerrainGrid->GetWidth() / 2,
                m_pTerrainGrid->GetHeight() / 2
                })
            : glm::vec2{
                SCREENWIDTH / RENDERSCALE / 2.0f,
                SCREENHEIGHT / RENDERSCALE / 2.0f
        };

        const float speed = PLAYER_MOVE_SPEED;

        glm::vec2 target = terrainCenter;

        glm::vec2 delta = target - pos;

        glm::vec2 moveDir{ 0.f, 0.f };

        const float eps = 0.5f;

        if (std::abs(delta.x) > eps)
        {
            moveDir.x = (delta.x > 0.f) ? 1.f : -1.f;
        }
        else if (std::abs(delta.y) > eps)
        {
            moveDir.y = (delta.y > 0.f) ? 1.f : -1.f;
        }
        else
        {
            parent->SetLocalPosition(terrainCenter);
            return m_pStatePool->Get<PlayerIdle>();
        }

        m_pMoveComponent->MoveObject(moveDir, speed);

        return nullptr;
	}

	void PlayerStart::Exit(PlayerControllerComponent& ctx)
	{
		//tell the game we are ready to rock n roll racing
        if (ctx.GetPrimaryPlayerFlag())
        {
		    EventManager::GetInstance().Publish(PlayerControllerComponent::PLAYERREADYHASH);
        }
	}
};