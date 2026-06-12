#ifndef _GAMEMANAGER_H_
#define _GAMEMANAGER_H_

#include <Singleton.h>
#include <EventManager.h>
#include <SceneManager.h>
#include <Components/ScoreComponent.h>
#include <Components/PlayerControllerComponent.h>

namespace dae
{
	struct GameData
	{
	public:
		bool m_IsSoloGame;
		bool m_IsVSGame;

		int m_Lives;
		int m_Score;
	};

	class GameManager : public Singleton<GameManager>
	{
	public:
		void StartSoloGame(const std::string& GameSceneName, const std::string& SubmitSceneName);
		void StartMPGame(const std::string& GameSceneName, const std::string& SubmitSceneName);
		void StartVSGame(const std::string& GameSceneName, const std::string& SubmitSceneName);

		GameData& GetGameData() { return m_Data; };

	private:
		friend class Singleton<GameManager>;

		GameManager()
		{
			//subscribe to the same event as the score component, so we can keep track of the score here as well
			m_ScoreEvent = EventManager::GetInstance().Subscribe(ScoreComponent::SCOREHASH,
				[this](unsigned int, const std::any& Data)
				{
					this->m_Data.m_Score += std::any_cast<uint32_t>(Data);
				});
			m_PlayerDeathEvent = EventManager::GetInstance().Subscribe(PlayerControllerComponent::PLAYERDEADHASH,
				[this](unsigned int, const std::any&)
				{
					this->m_Data.m_Lives--;
					if (m_Data.m_Lives == 0)
					{
						EventManager::GetInstance().Publish(SceneManager::CHANGELEVELHASH, m_HighScoreSubmitSceneName);
						return;
					}
					EventManager::GetInstance().Publish(SceneManager::CHANGELEVELHASH, m_GameSceneName);
				}
				);
		}

		void ResetPlayerData(const std::string& GameScene, const std::string& SubmitScene);

		static constexpr int LIVESONSTART{ 4 };
		std::string m_GameSceneName;
		std::string m_HighScoreSubmitSceneName;

		GameData m_Data{ 0 };
		EventManager::EventId m_ScoreEvent{0};
		EventManager::EventId m_PlayerDeathEvent{0};
	};
};

#endif