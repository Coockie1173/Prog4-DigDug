#ifndef _ENEMYWANDER_H_
#define _ENEMYWANDER_H_

#include <Components/EnemyStates/EnemyState.h>
#include <glm/glm.hpp>
#include <vector>

namespace dae
{
    class TerrainGridComponent;

    class EnemyWanderState final : public EnemyState
    {
    public:
        EnemyWanderState(StatePool<EnemyState>* pStatePool) : EnemyState(pStatePool) {};

        void Enter(IEnemyContext& ctx) override;
        EnemyState* Update(IEnemyContext& ctx) override;
        void Exit(IEnemyContext& ctx) override;

    private:
        static constexpr float WADDLESPEED         = 64.f;
        static constexpr float TIMEBETWEENFRAMES   = 0.2f;

        static constexpr float STUCK_CHECK_INTERVAL = 0.5f;
        static constexpr float STUCK_DISTANCE_SQ    = 4.f * 4.f;

        static constexpr float FIRE_CHECK_INTERVAL  = 2.5f;
        static constexpr float FIRE_CHANCE          = 0.40f;

        float m_WaddleTimer  { 0.f };
        int m_WaddleFrame  { 0   };

        glm::ivec2 m_CurrentCell{};
        glm::ivec2 m_TargetCell{};
        glm::vec2 m_Direction{ 1.f, 0.f };

        float m_StuckTimer{ 0.f };
        glm::vec2 m_LastCheckedPos{};

        float m_FireCheckTimer{ 0.f };

        ObjectMoveComponent* OMC{ nullptr };

        mutable TerrainGridComponent* m_pCachedTerrainGrid{ nullptr };

        TerrainGridComponent* GetTerrain(IEnemyContext& ctx) const;

        std::vector<glm::ivec2> GetOpenNeighbours(const glm::ivec2& cell, TerrainGridComponent* terrain) const;

        glm::ivec2 PickNextCell(const glm::ivec2& current, const glm::ivec2& cameFrom, TerrainGridComponent* terrain) const;
    };
}

#endif
