#ifndef _ENEMYFIRESTATE_H_
#define _ENEMYFIRESTATE_H_

#include <Components/EnemyStates/EnemyState.h>

namespace dae
{
    class EnemyFireState final : public EnemyState
    {
    public:
        EnemyFireState(StatePool<EnemyState>* pStatePool) : EnemyState(pStatePool) {};

        void Enter(IEnemyContext& ctx) override;
        EnemyState* Update(IEnemyContext& ctx) override;
        void Exit(IEnemyContext& ctx) override;

    private:
        static constexpr float FIRE_WIND_UP = 0.6f;

        float m_Timer{ 0.f };
        bool  m_FireSpawned{ false };
    };
}

#endif
