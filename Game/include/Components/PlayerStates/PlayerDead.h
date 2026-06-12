#ifndef _PLAYERDEADSTATE_H_
#define _PLAYERDEADSTATE_H_

#include "PlayerState.h"
#include <vector>
#include <Texture2D.h>
#include <cstdint>
#include <memory>

namespace dae
{
	class SwappableRenderComponent;
	class PlayerDead final : public PlayerState
	{
	public:
		PlayerDead(StatePool<PlayerState>* pStatePool) : PlayerState(pStatePool) {}

		void Enter(PlayerControllerComponent& Player) override;
		PlayerState* Update(PlayerControllerComponent& Player) override;
		void Exit(PlayerControllerComponent& Player) override;

	private:
		static constexpr float TIMEBETWEENFRAMES{ 0.2f };
		static constexpr uint8_t TOTALFRAMECOUNT{ 4 };
		uint8_t m_FrameNumber{ 0 };
		float m_FrametTimer{ 0 };
		std::vector<std::shared_ptr<Texture2D>> m_FrameTextures;
		SwappableRenderComponent* m_pRenderComponent{ nullptr };
	};
};

#endif
