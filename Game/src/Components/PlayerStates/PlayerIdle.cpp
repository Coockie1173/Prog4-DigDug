#include <Components/PlayerStates/PlayerIdle.h>
#include <Components/SwappableRenderComponent.h>
#include <Components/PlayerControllerComponent.h>
#include <GameObject.h>
#include <Components/PlayerStates/PlayerAttack.h>
#include <ResourceManager.h>

namespace dae
{
	const std::string PLAYER_IDLE{ "player/player_idle.png" };

	void PlayerIdle::Enter(PlayerControllerComponent& Player)
	{
		if (m_pRenderComponent == nullptr)
		{
			m_pRenderComponent = Player.GetPlayer()->GetComponent<SwappableRenderComponent>();
		}

		if (m_pIdleTexture == nullptr)
		{
			m_pIdleTexture = ResourceManager::GetInstance().LoadTexture(PLAYER_IDLE);
		}
		m_pRenderComponent->SetTexture(m_pIdleTexture);
	}

	PlayerState* PlayerIdle::Update(PlayerControllerComponent& Player)
	{		
		if (Player.GetIsAttacking())
		{
			return m_pStatePool->Get<PlayerAttack>();
		}
		return nullptr;
	}

	void PlayerIdle::Exit(PlayerControllerComponent&)
	{
		
	}
};
