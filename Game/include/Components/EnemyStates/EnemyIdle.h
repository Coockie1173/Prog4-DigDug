#ifndef _ENEMYIDLE_H_
#define _ENEMYIDLE_H_

#include "./EnemyState.h"
namespace dae
{
	class EnemyIdleState final : public EnemyState
	{
	public:
		EnemyIdleState(StatePool<EnemyState>* pStatePool) : EnemyState(pStatePool) {};

		void Enter(IEnemyContext& ctx) override;
		EnemyState* Update(IEnemyContext& ctx) override;
		void Exit(IEnemyContext& ctx) override;
	};
};

#endif