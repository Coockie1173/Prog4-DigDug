#include <GameManager.h>

namespace dae
{
	void GameManager::StartSoloGame(const std::string& GameScene, const std::string& SubmitScene, const std::string& ScoreName)
	{
		m_Data.m_IsSoloGame = true;
		m_Data.m_IsVSGame = false;
		ResetPlayerData(GameScene, SubmitScene, ScoreName);
	}

	void GameManager::StartMPGame(const std::string& GameScene, const std::string& SubmitScene, const std::string& ScoreName)
	{
		m_Data.m_IsSoloGame = false;
		m_Data.m_IsVSGame = false;
		ResetPlayerData(GameScene, SubmitScene, ScoreName);
	}

	void GameManager::StartVSGame(const std::string& GameScene, const std::string& SubmitScene, const std::string& ScoreName)
	{
		m_Data.m_IsSoloGame = false;
		m_Data.m_IsVSGame = true;
		ResetPlayerData(GameScene, SubmitScene, ScoreName);
	}

	void GameManager::ResetPlayerData(const std::string& GameScene, const std::string& SubmitScene, const std::string& ScoreName)
	{
		m_Data.m_Lives = LIVESONSTART;
		m_Data.m_Score = 0;
		m_Data.m_LevelNo = 0;
		m_Data.m_EnemyCount = 0;
		m_GameSceneName = GameScene;
		m_HighScoreSubmitSceneName = SubmitScene;
		m_HighScoreSceneName = ScoreName;
		m_Data.m_IsRespawn = false;
		m_Data.m_RockCrushCount = 0;
	}
};