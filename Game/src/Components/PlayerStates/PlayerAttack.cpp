#include <Components/PlayerStates/PlayerAttack.h>
#include <Components/SwappableRenderComponent.h>
#include <Components/PlayerControllerComponent.h>
#include <Components/PumpComponent.h>
#include <GameObject.h>
#include <Scene.h>
#include <Components/PlayerStates/PlayerIdle.h>
#include <Components/PlayerStates/PlayerMove.h>
#include <SoundSerivceLocator.h>
#include <ResourceManager.h>
#include <Texture2D.h>
#include <memory>

namespace dae
{
	const std::string PLAYER_ATTACK{ "player/player_attack.png" };
	const std::string PUMP_FRONT{ "player/pump_front.png" };
	constexpr float PUMP_TRAVEL_DISTANCE{ 64.0f };
	constexpr float PUMP_SPEED{ 128.0f };

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

			if (pumpFrontTexture == nullptr)
			{
				pumpFrontTexture = ResourceManager::GetInstance().LoadTexture(PUMP_FRONT);
			}

			const auto playerPosition = Player.GetPlayer()->GetWorldPosition();
			const auto facingDirection = Player.GetFacingVector();
			auto segment = std::make_unique<dae::GameObject>("PumpFront", playerPosition);
			auto* renderer = segment->AddComponent<SwappableRenderComponent>();
			renderer->SetTexture(pumpFrontTexture);
			renderer->SetFacingDirection(facingDirection);
			auto* pumpComponent = segment->AddComponent<PumpComponent>();
			pumpComponent->Configure(
				facingDirection,
				PUMP_TRAVEL_DISTANCE,
				PUMP_SPEED,
				[controllerPtr = &Player]()
				{
					controllerPtr->OnPlayerEndAttack();
				}
			);
			scene->QueueAdd(std::move(segment));
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
}