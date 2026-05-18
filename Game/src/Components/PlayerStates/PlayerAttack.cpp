#include <Components/PlayerStates/PlayerAttack.h>
#include <Components/SwappableRenderComponent.h>
#include <Components/PlayerControllerComponent.h>
#include <Components/ObjectMoveComponent.h>
#include <Components/PumpComponent.h>
#include <GameObject.h>
#include <Scene.h>
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

		auto* scene = Player.GetPlayer()->GetScene();
		if (scene != nullptr)
		{
			static std::shared_ptr<Texture2D> pumpFrontTexture{};
			static std::shared_ptr<Texture2D> pumpTailTexture{};

			if (pumpFrontTexture == nullptr)
			{
				pumpFrontTexture = ResourceManager::GetInstance().LoadTexture("player/pump_front.png");
			}
			if (pumpTailTexture == nullptr)
			{
				pumpTailTexture = ResourceManager::GetInstance().LoadTexture("player/pump_tail.png");
			}

			const auto playerPosition = Player.GetPlayer()->GetWorldPosition();
			const auto facingDirection = Player.GetFacingVector();

			auto pumpTail = std::make_unique<dae::GameObject>("PumpTail", playerPosition);
			auto* pumpTailRenderer = pumpTail->AddComponent<SwappableRenderComponent>();
			pumpTailRenderer->SetTexture(pumpTailTexture);

			auto* pumpTailObject = pumpTail.get();
			auto pumpFront = std::make_unique<dae::GameObject>("PumpFront", playerPosition + facingDirection * 16.0f);
			auto* pumpFrontRenderer = pumpFront->AddComponent<SwappableRenderComponent>();
			pumpFrontRenderer->SetTexture(pumpFrontTexture);
			pumpFront->AddComponent<ObjectMoveComponent>();
			auto* pumpComponent = pumpFront->AddComponent<PumpComponent>();
			pumpComponent->Configure(&Player, pumpTailObject, facingDirection, 64.0f, 128.0f);

			scene->Add(std::move(pumpTail));
			scene->Add(std::move(pumpFront));
		}
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