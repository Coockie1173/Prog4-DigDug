#include <Components/EnemyStates/EnemyIdle.h>
#include <Components/SwappableRenderComponent.h>
#include <Components/EnemyComponent.h>


namespace dae
{
	void EnemyIdleState::Enter(IEnemyContext& ctx)
	{
		ctx.GetRenderer()->SetTexture(ctx.GetTextureMap()[EnemyComponent::WalkFiles[0].hash]);
	}

	EnemyState* EnemyIdleState::Update(IEnemyContext&)
	{
		return nullptr;
	}

	void EnemyIdleState::Exit(IEnemyContext&)
	{

	}
};