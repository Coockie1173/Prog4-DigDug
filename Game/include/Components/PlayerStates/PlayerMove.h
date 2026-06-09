#ifndef _PLAYERMOVE_H_
#define _PLAYERMOVE_H_

#include "PlayerState.h"

namespace dae
{
	class SwappableRenderComponent;
	class ObjectMoveComponent;
	class Texture2D;
	class PlayerMove final : public PlayerState
	{
	public:
		PlayerMove(StatePool<PlayerState>* pStatePool) : PlayerState(pStatePool) {}
		void Enter(PlayerControllerComponent& Player) override;
		PlayerState* Update(PlayerControllerComponent& Player) override;
		void Exit(PlayerControllerComponent& Player) override;
	private:
		SwappableRenderComponent* m_pRenderComponent{ nullptr };
		ObjectMoveComponent* m_pMoveComponent{ nullptr };
		std::shared_ptr<Texture2D> m_pIdleTexture{ nullptr };
		std::shared_ptr<Texture2D> m_pWalkTexture{ nullptr };
		float WalkTimer{0};
	};
};

#endif // !_PLAYERMOVE_H_
