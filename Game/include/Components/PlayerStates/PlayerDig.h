#ifndef _PLAYERDIG_H_
#define _PLAYERDIG_H_

#include "PlayerState.h"
#include <glm/glm.hpp>
#include <memory>

namespace dae
{
	class SwappableRenderComponent;
	class ObjectMoveComponent;
	class TerrainGridComponent;
	class Texture2D;
	class PlayerDig final : public PlayerState
	{
	public:
		PlayerDig(StatePool<PlayerState>* pStatePool) : PlayerState(pStatePool) {}
		void Enter(PlayerControllerComponent& Player) override;
		PlayerState* Update(PlayerControllerComponent& Player) override;
		void Exit(PlayerControllerComponent& Player) override;
	private:
		glm::ivec2 m_DigTargetCell{};
		glm::vec2 m_DigEntryPos{};
		bool m_HasCarved{ false };

		static constexpr float CARVE_THRESHOLD = 0.80f;
		static constexpr float EXIT_THRESHOLD = 0.95f; 
		static constexpr float DIG_SPEED_FACTOR = 0.45f;

		SwappableRenderComponent* m_pRenderComponent{ nullptr };
		ObjectMoveComponent* m_pMoveComponent{ nullptr };
		TerrainGridComponent* m_pTerrainGrid{ nullptr };
		std::shared_ptr<Texture2D> m_pDigTexture{ nullptr };
	};
}

#endif
