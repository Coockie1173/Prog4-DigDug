#ifndef _ENEMYPOP_H_
#define _ENEMYPOP_H_

#include "./EnemyState.h"

namespace dae
{
	class EnemyPopState final : public EnemyState
	{
	public:
		EnemyPopState(StatePool<EnemyState>* pStatePool) : EnemyState(pStatePool) {};

		void Enter(IEnemyContext& ctx) override;
		EnemyState* Update(IEnemyContext& ctx) override;
		void Exit(IEnemyContext& ctx) override;
	};
};
#endif