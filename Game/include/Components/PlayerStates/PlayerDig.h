#ifndef _PLAYERDIG_H_
#define _PLAYERDIG_H_

#include "PlayerState.h"

namespace dae
{
	class SwappableRenderComponent;
	class ObjectMoveComponent;
	class TerrainGridComponent;
	class Texture2D;
	class PlayerDig final : public PlayerState
	{
	public:
		PlayerDig(PlayerStatePool* pStatePool) : PlayerState(pStatePool) {}
		void Enter(PlayerControllerComponent& Player) override;
		PlayerState* Update(PlayerControllerComponent& Player) override;
		void Exit(PlayerControllerComponent& Player) override;
	private:
		SwappableRenderComponent* m_pRenderComponent{ nullptr };
		ObjectMoveComponent* m_pMoveComponent{ nullptr };
		TerrainGridComponent* m_pTerrainGrid{ nullptr };
		std::shared_ptr<Texture2D> m_pDigTexture{ nullptr };
		float m_DigTimer{ 0.0f };
	};
}

#endif
