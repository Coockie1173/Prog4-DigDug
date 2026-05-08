#ifndef _PLAYER_ATTACK_H_
#define _PLAYER_ATTACK_H_

#include "PlayerState.h"
#include <memory>

class SwappableRenderComponent;
namespace dae
{
    class Texture2D;
    class PlayerAttack : public PlayerState
    {
    public:
		PlayerAttack(PlayerStatePool* pStatePool) : PlayerState(pStatePool) {}
        void Enter(PlayerControllerComponent& Player) override;
        PlayerState* Update(PlayerControllerComponent& Player) override;
        void Exit(PlayerControllerComponent& Player) override;
    private:
        SwappableRenderComponent* m_pRenderComponent{ nullptr };
		std::shared_ptr<Texture2D> m_pAttackTexture{ nullptr };
    };
}

#endif