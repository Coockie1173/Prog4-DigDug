#include <Components/EnemyStates/EnemyWander.h>
#include <Components/EnemyStates/EnemyGhost.h>
#include <Components/EnemyStates/EnemyFireState.h>
#include <Components/SwappableRenderComponent.h>
#include <Components/EnemyComponent.h>
#include <Timing.h>
#include <Components/ObjectMoveComponent.h>
#include <Components/TerrainGridComponent.h>
#include <GameObject.h>
#include <SceneManager.h>

#include <cstdlib>

namespace dae
{
    void EnemyWanderState::Enter(IEnemyContext& ctx)
    {
        m_WaddleTimer   = 0.f;
        m_FireCheckTimer = 0.f;

        ctx.GetRenderer()->SetTexture(ctx.GetTextureMap()[EnemyComponent::WalkFiles[m_WaddleFrame].hash]);

        OMC = ctx.GetOMC();

        auto* terrain = GetTerrain(ctx);
        const glm::vec2 worldPos = ctx.GetMe()->GetParent()->GetWorldPosition();

        m_CurrentCell = terrain ? terrain->WorldToCell(worldPos) : glm::ivec2{ 0, 0 };
        m_TargetCell = m_CurrentCell;

        m_StuckTimer = 0.f;
        m_LastCheckedPos = worldPos;
    }

    EnemyState* EnemyWanderState::Update(IEnemyContext& ctx)
    {
        const float dt = Timing::GetInstance().GetDeltaTime();

        m_WaddleTimer += dt;
        if (m_WaddleTimer > TIMEBETWEENFRAMES)
        {
            m_WaddleTimer = 0.f;
            m_WaddleFrame ^= 1;
            ctx.GetRenderer()->SetTexture(
                ctx.GetTextureMap()[EnemyComponent::WalkFiles[m_WaddleFrame].hash]);
        }

        if (m_Direction.x != 0.f)
        {
            ctx.GetRenderer()->SetFacingDirection({ m_Direction.x > 0.f ? 1.f : -1.f, 0.f });
            ctx.SetFacing({ m_Direction.x > 0.f ? 1.f : -1.f, 0.f });
        }

        auto* terrain = GetTerrain(ctx);
        if (!terrain) return nullptr;

        auto* parent = ctx.GetMe()->GetParent();
        const glm::vec2 worldPos = parent->GetWorldPosition();
        const glm::vec2 targetWorld = terrain->CellToWorldCenter(m_TargetCell);
        const glm::vec2 toTarget = targetWorld - worldPos;
        const float distSq = glm::dot(toTarget, toTarget);
        const float stepDist = WADDLESPEED * dt;

        if (distSq <= stepDist * stepDist)
        {
            parent->SetLocalPosition(targetWorld);

            const glm::ivec2 cameFrom = m_CurrentCell;
            m_CurrentCell = m_TargetCell;
            m_TargetCell  = PickNextCell(m_CurrentCell, cameFrom, terrain);
        }
        else if (distSq > 0.0001f)
        {
            m_Direction = glm::normalize(toTarget);
            OMC->MoveObject(m_Direction, WADDLESPEED);
        }

        m_StuckTimer += dt;
        if (m_StuckTimer >= STUCK_CHECK_INTERVAL)
        {
            m_StuckTimer = 0.f;
            const glm::vec2 currentPos = parent->GetWorldPosition();
            const glm::vec2 moved      = currentPos - m_LastCheckedPos;
            m_LastCheckedPos = currentPos;

            if (glm::dot(moved, moved) < STUCK_DISTANCE_SQ)
            {
                return m_pStatePool->Get<EnemyGhostState>();
            }
        }

        if (ctx.CanAttack())
        {
            m_FireCheckTimer += dt;
            if (m_FireCheckTimer >= FIRE_CHECK_INTERVAL)
            {
                m_FireCheckTimer = 0.f;
                const float roll = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
                if (roll < FIRE_CHANCE)
                {
                    return m_pStatePool->Get<EnemyFireState>();
                }
            }
        }

        return nullptr;
    }

    void EnemyWanderState::Exit(IEnemyContext& ctx)
    {
        (void)ctx;
    }

    dae::TerrainGridComponent* dae::EnemyWanderState::GetTerrain(IEnemyContext& ctx) const
    {
        if (m_pCachedTerrainGrid != nullptr)
            return m_pCachedTerrainGrid;

        auto* enemy = ctx.GetMe()->GetParent();
        if (!enemy) return nullptr;

        auto* scene = enemy->GetScene();
        if (!scene) return nullptr;

        auto* terrainObject = scene->FindGameObject("Terrain");
        if (!terrainObject) return nullptr;

        m_pCachedTerrainGrid = terrainObject->GetComponent<dae::TerrainGridComponent>();
        return m_pCachedTerrainGrid;
    }

    std::vector<glm::ivec2> EnemyWanderState::GetOpenNeighbours(const glm::ivec2& cell, TerrainGridComponent* terrain) const
    {
        static const std::pair<glm::ivec2, WallFlags> kNeighbours[4] =
        {
            { { 0, -1}, WallFlags::Top },
            { { 0, 1}, WallFlags::Bottom },
            { {-1, 0}, WallFlags::Left },
            { { 1, 0}, WallFlags::Right },
        };

        if (terrain->IndexOf(cell) < 0) return {};

        const uint8_t walls = terrain->GetCellWalls(cell);
        std::vector<glm::ivec2> open;

        for (auto& [delta, flag] : kNeighbours)
        {
            if ((walls & static_cast<uint8_t>(flag)) == 0)
            {
                const glm::ivec2 neighbour = cell + delta;
                if (terrain->IsValidCell(neighbour))
                    open.push_back(neighbour);
            }
        }

        return open;
    }

    glm::ivec2 EnemyWanderState::PickNextCell(const glm::ivec2& current, const glm::ivec2& cameFrom, TerrainGridComponent* terrain) const
    {
        auto neighbours = GetOpenNeighbours(current, terrain);
        if (neighbours.empty()) return current;

        std::vector<glm::ivec2> preferred, fallback;
        for (auto& n : neighbours)
        {
            if (n == cameFrom) fallback.push_back(n);
            else preferred.push_back(n);
        }

        auto& pool = preferred.empty() ? fallback : preferred;

        const glm::ivec2 forward = current + (current - cameFrom);
        for (auto& n : pool)
            if (n == forward) return n;

        return pool[static_cast<size_t>(rand()) % pool.size()];
    }
}
