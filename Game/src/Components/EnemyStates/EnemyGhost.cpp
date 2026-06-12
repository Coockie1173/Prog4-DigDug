#include <Components/EnemyStates/EnemyGhost.h>
#include <Components/EnemyStates/EnemyWander.h> 
#include <Components/EnemyComponent.h>
#include <Components/TerrainGridComponent.h>
#include <Components/PlayerControllerComponent.h>
#include <Components/ObjectMoveComponent.h>
#include <Components/SwappableRenderComponent.h>
#include <GameObject.h>
#include <Scene.h>
#include <Timing.h>
#include <StateHelper.h>
#include <optional>

namespace dae
{
    void EnemyGhostState::Enter(IEnemyContext& ctx)
    {
        m_pCachedTerrainGrid = nullptr;
        m_HasCrossedDirt = false;
        m_Direction = {};

        auto* terrain = GetTerrain(ctx);
        if (terrain)
        {
            const glm::vec2 worldPos = ctx.GetMe()->GetParent()->GetWorldPosition();
            m_LastCell = terrain->WorldToCell(worldPos);
        }
        else
        {
            m_LastCell = { -1, -1 };
        }

        ctx.GetRenderer()->SetTexture(
            ctx.GetTextureMap()[EnemyComponent::GhostFiles[0].hash]
        );
    }

    EnemyState* EnemyGhostState::Update(IEnemyContext& ctx)
    {
        auto* terrain = GetTerrain(ctx);
        auto* parent = ctx.GetMe()->GetParent();

        const auto playerPosOpt = GetNearestPlayerPosition(ctx);
        if (playerPosOpt.has_value())
        {
            const glm::vec2 toPlayer = playerPosOpt.value() - parent->GetWorldPosition();
            const float lenSq = glm::dot(toPlayer, toPlayer);
            if (lenSq > 0.0001f)
                m_Direction = glm::normalize(toPlayer);
        }

        if (glm::dot(m_Direction, m_Direction) > 0.0001f)
            ctx.GetOMC()->MoveObject(m_Direction, GHOST_SPEED);

        if (terrain)
        {
            const glm::vec2   worldPos = parent->GetWorldPosition();
            const glm::ivec2  currentCell = terrain->WorldToCell(worldPos);

            if (currentCell != m_LastCell && terrain->IsValidCell(currentCell))
            {
                const uint8_t depth = terrain->GetCellDepth(currentCell);
                const uint8_t walls = terrain->GetCellWalls(currentCell);

                const bool allWallsSet = (walls == static_cast<uint8_t>(WallFlags::Top | WallFlags::Bottom | WallFlags::Left | WallFlags::Right));
                const bool isSolidDirt = (depth > 0) || allWallsSet;

                if (isSolidDirt)
                {
                    m_HasCrossedDirt = true;
                }
                else if (m_HasCrossedDirt)
                {
                    const glm::vec2 cellCenter = terrain->CellToWorldCenter(currentCell);
                    parent->SetLocalPosition(cellCenter);
                    m_LastCell = currentCell;
                    return m_pStatePool->Get<EnemyWanderState>();
                }

                m_LastCell = currentCell;
            }
        }

        return nullptr;
    }

    void EnemyGhostState::Exit(IEnemyContext&)
    {

    }

    TerrainGridComponent* EnemyGhostState::GetTerrain(IEnemyContext& ctx) const
    {
        if (m_pCachedTerrainGrid)
            return m_pCachedTerrainGrid;

        auto* parent = ctx.GetMe()->GetParent();
        if (!parent) return nullptr;

        auto* scene = parent->GetScene();
        if (!scene)  return nullptr;

        auto* terrainObj = scene->FindGameObject("Terrain");
        if (!terrainObj) return nullptr;

        m_pCachedTerrainGrid = terrainObj->GetComponent<TerrainGridComponent>();
        return m_pCachedTerrainGrid;
    }

    std::optional<glm::vec2> EnemyGhostState::GetNearestPlayerPosition(IEnemyContext& ctx) const
    {
        auto* parent = ctx.GetMe()->GetParent();
        if (!parent) return std::nullopt;

        auto* scene = parent->GetScene();
        if (!scene)  return std::nullopt;

        const glm::vec2 myPos = parent->GetWorldPosition();
        std::optional<glm::vec2> nearest;
        float bestDistSq = std::numeric_limits<float>::max();

        scene->ForEachGameObject([&](GameObject* obj)
            {
                auto* pcc = obj->GetComponent<PlayerControllerComponent>();
                if (!pcc) return;

                const glm::vec2 pPos = obj->GetWorldPosition();
                const glm::vec2 delta = pPos - myPos;
                const float     distSq = glm::dot(delta, delta);

                if (distSq < bestDistSq)
                {
                    bestDistSq = distSq;
                    nearest = pPos;
                }
            });

        return nearest;
    }
}