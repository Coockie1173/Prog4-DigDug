#ifndef _PLAYERIDLE_H_
#define _PLAYERIDLE_H_

#include "PlayerState.h"
#include <Texture2D.h>

class SwappableRenderComponent;
namespace dae
{
	class Texture2D;
	class PlayerIdle final : public PlayerState
	{
	public:
		PlayerIdle(PlayerStatePool* pStatePool) : PlayerState(pStatePool) {}
		void Enter(PlayerControllerComponent& Player) override;
		PlayerState* Update(PlayerControllerComponent& Player) override;
		void Exit(PlayerControllerComponent& Player) override;
	private:
		SwappableRenderComponent* m_pRenderComponent{ nullptr };
		std::shared_ptr<Texture2D> m_pIdleTexture{ nullptr };
	};
};

#endif