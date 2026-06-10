#ifndef _ENEMYPUMPED_H_
#define _ENEMYPUMPED_H_

#include "./EnemyState.h"
namespace dae
{
	class PlayerControllerComponent;
	class SwappableRenderComponent;
	class EnemyPumpedState final : public EnemyState
	{
	public:
		EnemyPumpedState(StatePool<EnemyState>* pStatePool) : EnemyState(pStatePool) {};

		void Enter(IEnemyContext& ctx) override;
		EnemyState* Update(IEnemyContext& ctx) override;
		void Exit(IEnemyContext& ctx) override;

		void SetPumper(PlayerControllerComponent* pumper) { m_pPumper = pumper; };
		void OnAirPumped();

	private:
		PlayerControllerComponent* m_pPumper{nullptr};
		int8_t m_PumpCount{0};
		static constexpr float TIMETODEFLATE{ .5f };
		float m_DeflateTimer{ 0 };
		SwappableRenderComponent* m_pSRC{nullptr};
	};
};

#endif