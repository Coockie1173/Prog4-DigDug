#include <Components/PlayerStates/PlayerPumping.h>
#include <Components/PlayerControllerComponent.h>
#include <Components/SwappableRenderComponent.h>
#include <GameObject.h>
#include <Timing.h>
#include <Components/EnemyComponent.h>

namespace dae
{
	void PlayerPumping::Enter(PlayerControllerComponent& Player)
	{
		if (m_pRenderComponent == nullptr)
		{
			m_pRenderComponent = Player.GetPlayer()->GetComponent<SwappableRenderComponent>();
		}
	}

	PlayerState* PlayerPumping::Update(PlayerControllerComponent&)
	{
		m_PumpTimer -= Timing::GetInstance().GetDeltaTime();
		return nullptr;
	}

	void PlayerPumping::Exit(PlayerControllerComponent&)
	{

	}

	void PlayerPumping::Pump()
	{
		if (m_PumpTimer < 0)
		{
			m_PumpTimer = TIMEBETWEENPUMPS;
			m_pTarget->OnAirBlownIntoEnemy();
		}
	}
};