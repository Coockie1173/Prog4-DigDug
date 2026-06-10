#ifndef _PLAYERPUMPING_H_
#define _PLAYERPUMPING_H_

#include "PlayerState.h"

namespace dae
{
	class SwappableRenderComponent;
	class ObjectMoveComponent;
	class EnemyComponent;
	class Texture2D;
	class PlayerPumping final : public PlayerState
	{
	public:
		PlayerPumping(StatePool<PlayerState>* pStatePool) : PlayerState(pStatePool) {}
		void Enter(PlayerControllerComponent& Player) override;
		PlayerState* Update(PlayerControllerComponent& Player) override;
		void Exit(PlayerControllerComponent& Player) override;

		void SetTarget(EnemyComponent* Enemy) { m_pTarget = Enemy; };
		void Pump();
	private:
		SwappableRenderComponent* m_pRenderComponent{ nullptr };
		std::shared_ptr<Texture2D> m_pPumpTextureOne{ nullptr };
		std::shared_ptr<Texture2D> m_pPumpTextureTwo{ nullptr };
		EnemyComponent* m_pTarget{ nullptr };
		static constexpr float TIMEBETWEENPUMPS{ .25f };
		float m_PumpTimer{ 0 };
	};
};

#endif