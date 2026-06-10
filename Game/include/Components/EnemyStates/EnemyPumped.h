#ifndef _ENEMYPUMPED_H_
#define _ENEMYPUMPED_H_

#include "./EnemyState.h"
namespace dae
{
	class PlayerControllerComponent;
	class EnemyPumpedState final : public EnemyState
	{
	public:
		EnemyPumpedState(StatePool<EnemyState>* pStatePool) : EnemyState(pStatePool) {};

		void Enter(IEnemyContext& ctx) override;
		EnemyState* Update(IEnemyContext& ctx) override;
		void Exit(IEnemyContext& ctx) override;

		void SetPumper(PlayerControllerComponent* pumper) { m_pPumper = pumper; };

	private:
		PlayerControllerComponent* m_pPumper{nullptr};
	};
};

#endif