#include <Components/PlayerStates/PlayerAttack.h>
#include <Components/SwappableRenderComponent.h>
#include <Components/PlayerControllerComponent.h>
#include <GameObject.h>
#include <Components/PlayerStates/PlayerIdle.h>
#include <Components/PlayerStates/PlayerMove.h>
#include <SoundSerivceLocator.h>
#include <ResourceManager.h>
#include <Texture2D.h>

namespace dae
{
	const std::string PLAYER_ATTACK{ "player/player_attack.png" };

	void PlayerAttack::Enter(PlayerControllerComponent& Player)
	{
		if (m_pRenderComponent == nullptr)
		{
			m_pRenderComponent = Player.GetPlayer()->GetComponent<SwappableRenderComponent>();
		}
		if (m_pAttackTexture == nullptr)
		{
			m_pAttackTexture = ResourceManager::GetInstance().LoadTexture(PLAYER_ATTACK);
		}
		m_pRenderComponent->SetTexture(m_pAttackTexture);
		SoundServiceLocator::GetSoundSystem().PlaySound("Data/sound/Shoot.wav");
	}

	PlayerState* PlayerAttack::Update(PlayerControllerComponent& Player)
	{
		if (!Player.GetIsAttacking())
		{
			if (Player.GetMoveIntent() != glm::vec2{})
			{
				return m_pStatePool->Get<PlayerMove>();
			}
			return m_pStatePool->Get<PlayerIdle>();
		}
		return nullptr;
	}

	void PlayerAttack::Exit(PlayerControllerComponent&)
	{

	}
};