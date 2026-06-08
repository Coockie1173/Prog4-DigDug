#ifndef _PLAYER_START_H_
#define _PLAYER_START_H_

#include "PlayerState.h"
#include <memory.h>

namespace dae
{
	class TerrainGridComponent;
	class ObjectMoveComponent;
	class SwappableRenderComponent;
	class Texture2D;
	class PlayerStart final : public PlayerState
	{
	public:
		PlayerStart(PlayerStatePool* pStatePool) : PlayerState(pStatePool) {}
		void Enter(PlayerControllerComponent& Player) override;
		PlayerState* Update(PlayerControllerComponent& Player) override;
		void Exit(PlayerControllerComponent& Player) override;
	private:
		SwappableRenderComponent* m_pRenderComponent{ nullptr };
		ObjectMoveComponent* m_pMoveComponent{ nullptr };
		std::shared_ptr<Texture2D> m_pIdleTexture{ nullptr };
		std::shared_ptr<Texture2D> m_pWalkTexture{ nullptr };
		float WalkTimer{ 0 };
		TerrainGridComponent* m_pTerrainGrid{ nullptr };
	};
};

#endif