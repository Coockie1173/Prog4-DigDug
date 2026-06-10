#include <Components/PlayerStates/PlayerPumping.h>
#include <Components/PlayerControllerComponent.h>
#include <Components/SwappableRenderComponent.h>
#include <GameObject.h>

namespace dae
{
	void PlayerPumping::Enter(PlayerControllerComponent& Player)
	{
		if (m_pRenderComponent == nullptr)
		{
			m_pRenderComponent = Player.GetPlayer()->GetComponent<SwappableRenderComponent>();
		}
	}

	PlayerState* PlayerPumping::Update(PlayerControllerComponent&)
	{
		return nullptr;
	}

	void PlayerPumping::Exit(PlayerControllerComponent&)
	{

	}

};