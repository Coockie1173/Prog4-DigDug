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
#include <Config.h>
#include <cmath>

namespace dae
{
    const std::string PLAYER_DIG_TEX{ "player/player_dig.png" };

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
            m_pDigTexture = ResourceManager::GetInstance().LoadTexture(PLAYER_DIG_TEX);
        }
        if (m_pRenderComponent != nullptr)
        {
            m_pRenderComponent->SetTexture(m_pDigTexture);
        }

        const glm::vec2 pos = Player.GetPlayer()->GetWorldPosition();
        const glm::vec2 facing = Player.GetFacingVector();
        m_DigEntryPos = pos;
        m_HasCarved = false;

        if (m_pTerrainGrid != nullptr)
        {
            const auto currentCell = m_pTerrainGrid->WorldToCell(pos);
            m_DigTargetCell = currentCell + glm::ivec2(glm::round(facing));
        }
    }

    PlayerState* PlayerDig::Update(PlayerControllerComponent& Player)
    {
        if (m_pTerrainGrid == nullptr)
        {
            return m_pStatePool->Get<PlayerIdle>();
        }

        const glm::vec2 pos = Player.GetPlayer()->GetWorldPosition();
        const glm::vec2 facing = Player.GetFacingVector();

        const glm::vec2 expectedFacing = glm::vec2(m_DigTargetCell - m_pTerrainGrid->WorldToCell(m_DigEntryPos));
        if (glm::dot(facing, expectedFacing) <= 0.0f)
        {
            if (Player.GetMoveIntent() != glm::vec2{})
            {
                return m_pStatePool->Get<PlayerMove>();
            }
            return m_pStatePool->Get<PlayerIdle>();
        }

        const float penetration = glm::dot(pos - m_DigEntryPos, facing);
        const float cellSize = m_pTerrainGrid->GetCellSize();

        if (!m_HasCarved && penetration >= cellSize * CARVE_THRESHOLD)
        {
            m_pTerrainGrid->CarveCell(m_DigTargetCell);
            m_HasCarved = true;
        }

        glm::vec2 digDir = facing;
        m_pMoveComponent->MoveObject(digDir, PLAYER_MOVE_SPEED * DIG_SPEED_FACTOR);

        if (penetration >= cellSize * EXIT_THRESHOLD)
        {
            if (Player.GetMoveIntent() != glm::vec2{})
            {
                return m_pStatePool->Get<PlayerMove>();
            }
            return m_pStatePool->Get<PlayerIdle>();
        }

        return nullptr;
    }

    void PlayerDig::Exit(PlayerControllerComponent& Player)
    {
        Player.OnPlayerEndDig();
    }
}