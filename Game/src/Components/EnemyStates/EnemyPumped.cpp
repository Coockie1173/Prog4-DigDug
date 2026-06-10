#include <Components/EnemyStates/EnemyPumped.h>

namespace dae
{
	void EnemyPumpedState::Enter(IEnemyContext& ctx)
	{
		(void)ctx;
	}

	EnemyState* EnemyPumpedState::Update(IEnemyContext& ctx)
	{
		(void)ctx;
		return nullptr;
	}

	void EnemyPumpedState::Exit(IEnemyContext& ctx)
	{
		(void)ctx;
	}
};
