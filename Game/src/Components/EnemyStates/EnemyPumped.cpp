#include <Components/EnemyStates/EnemyPumped.h>
#include <Components/EnemyStates/EnemyWander.h>
#include <Components/EnemyComponent.h>
#include <Components/SwappableRenderComponent.h>
#include <Components/PlayerControllerComponent.h>
#include <Timing.h>
#include <Components/EnemyStates/EnemyPop.h>

namespace dae
{
	void EnemyPumpedState::Enter(IEnemyContext& ctx)
	{
		m_PumpCount = 0;
		m_DeflateTimer = TIMETODEFLATE;
		m_pSRC = ctx.GetRenderer();
	}

	EnemyState* EnemyPumpedState::Update(IEnemyContext& ctx)
	{
		m_DeflateTimer -= Timing::GetInstance().GetDeltaTime();
		if (m_DeflateTimer < 0)
		{
			m_PumpCount--;
			m_DeflateTimer = TIMETODEFLATE;
		}
		if (m_PumpCount < 0)
		{
			return m_pStatePool->Get<EnemyWanderState>();
		}

		m_pSRC->SetTexture(ctx.GetTextureMap()[EnemyComponent::PumpFiles[m_PumpCount / 2].hash]);
		if (m_PumpCount == 7)
		{
			//time to die nerd
			return m_pStatePool->Get<EnemyPopState>(); //<- I realise this could just be a call to killme instead, but too late now!
		}

		return nullptr;
	}

	void EnemyPumpedState::Exit(IEnemyContext&)
	{
		m_pPumper->AlertPumperDone();
	}

	void EnemyPumpedState::OnAirPumped()
	{
		m_PumpCount++;
		m_DeflateTimer = TIMETODEFLATE;
	}
};
