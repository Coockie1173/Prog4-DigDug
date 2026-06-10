#include <Components/EnemyStates/EnemyPop.h>
#include <Components/EnemyComponent.h>
#include <Components/SwappableRenderComponent.h>

namespace dae
{
	void EnemyPopState::Enter(IEnemyContext& ctx)
	{
		ctx.KillMe();
	}

	EnemyState* EnemyPopState::Update(IEnemyContext&)
	{
		return nullptr;
	}

	void EnemyPopState::Exit(IEnemyContext&)
	{

	}
};