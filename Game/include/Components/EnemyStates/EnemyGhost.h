#ifndef _ENEMYGHOST_H_
#define _ENEMYGHOST_H_

#include <Components/EnemyStates/EnemyState.h>
#include <glm/vec2.hpp>
#include <optional>

namespace dae
{
    class TerrainGridComponent;
    class PlayerControllerComponent;

    class EnemyGhostState final : public EnemyState
    {
    public:
        using EnemyState::EnemyState;

        void Enter(IEnemyContext& ctx) override;
        EnemyState* Update(IEnemyContext& ctx) override;
        void Exit(IEnemyContext& ctx) override;

    private:
        TerrainGridComponent* GetTerrain(IEnemyContext& ctx) const;

        std::optional<glm::vec2> GetNearestPlayerPosition(IEnemyContext& ctx) const;

        mutable TerrainGridComponent* m_pCachedTerrainGrid{ nullptr };

        glm::vec2 m_Direction{};
        glm::ivec2 m_LastCell{ -1, -1 };
        bool m_HasCrossedDirt{ false };

        static constexpr float GHOST_SPEED = 60.f;
    };
}

#endif