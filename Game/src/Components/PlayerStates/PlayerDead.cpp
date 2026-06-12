#include <Components/PlayerStates/PlayerDead.h>

#include <EventManager.h>
#include <Components/PlayerControllerComponent.h>
#include <Timing.h>
#include <ResourceManager.h>
#include <GameObject.h>
#include <SwappableRenderComponent.h>

namespace dae
{
	void PlayerDead::Enter(PlayerControllerComponent& Player)
	{
		//I know doing it like this is dookie from the butt, if I have time (which I won't) I'll patch it like I did with the enemies
		auto frame = ResourceManager::GetInstance().LoadTexture("player/player_death1.png");
		m_FrameTextures.push_back(std::move(frame));
		frame = ResourceManager::GetInstance().LoadTexture("player/player_death2.png");
		m_FrameTextures.push_back(std::move(frame));
		frame = ResourceManager::GetInstance().LoadTexture("player/player_death3.png");
		m_FrameTextures.push_back(std::move(frame));
		frame = ResourceManager::GetInstance().LoadTexture("player/player_death4.png");
		m_FrameTextures.push_back(std::move(frame));
		frame = ResourceManager::GetInstance().LoadTexture("player/player_death5.png");
		m_FrameTextures.push_back(std::move(frame));

		if (m_pRenderComponent == nullptr)
		{
			m_pRenderComponent = Player.GetPlayer()->GetComponent<SwappableRenderComponent>();
		}
	}

	PlayerState* PlayerDead::Update(PlayerControllerComponent&)
	{
		if (m_FrameNumber == 255) //stall until scene reload has completed
		{
			return nullptr;
		}

		m_FrametTimer += Timing::GetInstance().GetDeltaTime();

		if (m_FrametTimer > TIMEBETWEENFRAMES)
		{
			m_FrametTimer = 0;
			m_FrameNumber++;
		}

		if (m_FrameNumber > TOTALFRAMECOUNT)
		{
			EventManager::GetInstance().Publish(PlayerControllerComponent::PLAYERDEADHASH);
			m_FrameNumber = 255;
			return nullptr;
		}

		m_pRenderComponent->SetTexture(m_FrameTextures[m_FrameNumber]);

		return nullptr;
	}

	void PlayerDead::Exit(PlayerControllerComponent&)
	{

	}
};