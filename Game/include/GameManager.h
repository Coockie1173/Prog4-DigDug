#ifndef _GAMEMANAGER_H_
#define _GAMEMANAGER_H_

#include <Singleton.h>
#include <EventManager.h>
#include <SceneManager.h>
#include <Components/ScoreComponent.h>
#include <Components/PlayerControllerComponent.h>
#include <optional>
#include <Components/EnemyComponent.h>
#include <fstream>
#include <filesystem>
#include <ranges>
#include <vector>
#include <string>
#include <InputManager.h>


namespace dae
{
	struct TerrainSnapshot
	{
		TerrainSnapshot() = default;
		std::vector<uint8_t> cells;
		std::vector<uint8_t> cellWalls;
		int width{ 0 };
		int height{ 0 };
	};

	struct GameData
	{
	public:
		GameData() = default;
		bool m_IsSoloGame{ false };
		bool m_IsVSGame{ false };
		bool m_IsRespawn{ false };

		int m_Lives{ 0 };
		int m_LevelNo{ 0 };
		int m_Score{ 0 };
		int m_EnemyCount{ 0 };
		int m_RockCrushCount{ 0 };

		std::optional<TerrainSnapshot> m_TerrainSnapshot{};
	};

	class GameManager : public Singleton<GameManager>
	{
	public:
		void StartSoloGame(const std::string& GameSceneName, const std::string& SubmitSceneName, const std::string& ScoreName);
		void StartMPGame(const std::string& GameSceneName, const std::string& SubmitSceneName, const std::string& ScoreName);
		void StartVSGame(const std::string& GameSceneName, const std::string& SubmitSceneName, const std::string& ScoreName);

		GameData& GetGameData() { return m_Data; };

		static constexpr auto SUBMITSCOREHASH = make_sdbm_hash("Player Submits Score");

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
					if (m_Data.m_Lives <=  0)
					{
						EventManager::GetInstance().Publish(SceneManager::CHANGELEVELHASH, m_HighScoreSubmitSceneName);
						return;
					}
					m_Data.m_IsRespawn = true;
					EventManager::GetInstance().Publish(SceneManager::CHANGELEVELHASH, m_GameSceneName);
				}
				);
			m_OnEnemySpawn = EventManager::GetInstance().Subscribe(EnemyComponent::ENEMYSPAWNHASH,
				[this](unsigned int, const std::any&) {
					m_Data.m_EnemyCount++;
				});
			m_OnEnemyDeath = EventManager::GetInstance().Subscribe(EnemyComponent::ENEMYDEATHHASH,
				[this](unsigned int, const std::any&) {
					m_Data.m_EnemyCount--;
					if (m_Data.m_EnemyCount == 0)
					{
						if ((++m_Data.m_LevelNo) == 3)
						{
							EventManager::GetInstance().Publish(SceneManager::CHANGELEVELHASH, m_HighScoreSubmitSceneName);
							return;
						}
						m_Data.m_IsRespawn = false;
						EventManager::GetInstance().Publish(SceneManager::CHANGELEVELHASH, m_GameSceneName);
					}
				});
			m_SubmitScoreEvent = EventManager::GetInstance().Subscribe(GameManager::SUBMITSCOREHASH,
				[this](unsigned int, const std::any& Data)
				{
					const std::string playerName{ std::any_cast<std::string>(Data) };

					const std::filesystem::path scoreFile{ "./Data/Scores.txt" };

					std::vector<std::pair<std::string, int>> scores;

					{
						std::ifstream input{ scoreFile };

						std::string name;
						int score{};
						while (input >> name >> score)
						{
							scores.emplace_back(std::move(name), score);
						}
					}

					const auto insertPos = std::ranges::find_if(
						scores,
						[this](const auto& entry)
						{
							return m_Data.m_Score > entry.second;
						});

					if (insertPos == scores.end() && scores.size() >= 10)
						return;

					scores.insert(
						insertPos,
						{ playerName, m_Data.m_Score });

					if (scores.size() > 10)
						scores.resize(10);

					std::ofstream output{ scoreFile, std::ios::trunc };

					for (const auto& [name, score] : scores)
					{
						output << name << '\t' << score << '\n';
					}

					EventManager::GetInstance().Publish(SceneManager::CHANGELEVELHASH, m_HighScoreSceneName);
				});
			m_KILLKILLKILLEvent = EventManager::GetInstance().Subscribe(InputManager::ENEMYKILLDEBUGKILLHASH,
				[this](unsigned int, const std::any&)
				{
					for (int i = m_Data.m_EnemyCount; i >= 0; --i)
					{
						EventManager::GetInstance().Publish(EnemyComponent::ENEMYDEATHHASH);
					}
				}
				);
		}

		void ResetPlayerData(const std::string& GameScene, const std::string& SubmitScene, const std::string& ScoreName);

		static constexpr int LIVESONSTART{ 4 };
		std::string m_GameSceneName;
		std::string m_HighScoreSubmitSceneName;
		std::string m_HighScoreSceneName;

		GameData m_Data{};
		EventManager::EventId m_ScoreEvent{0};
		EventManager::EventId m_PlayerDeathEvent{0};
		EventManager::EventId m_OnEnemySpawn{ 0 };
		EventManager::EventId m_OnEnemyDeath{ 0 };
		EventManager::EventId m_SubmitScoreEvent{ 0 };
		EventManager::EventId m_KILLKILLKILLEvent{ 0 };
	};
};

#endif