#include <GameManager.h>

namespace dae
{
	void GameManager::StartSoloGame(const std::string& GameScene, const std::string& SubmitScene)
	{
		m_Data.m_IsSoloGame = true;
		m_Data.m_IsVSGame = false;
		ResetPlayerData(GameScene, SubmitScene);
	}

	void GameManager::StartMPGame(const std::string& GameScene, const std::string& SubmitScene)
	{
		m_Data.m_IsSoloGame = false;
		m_Data.m_IsVSGame = false;
		ResetPlayerData(GameScene, SubmitScene);
	}

	void GameManager::StartVSGame(const std::string& GameScene, const std::string& SubmitScene)
	{
		m_Data.m_IsSoloGame = false;
		m_Data.m_IsVSGame = true;
		ResetPlayerData(GameScene, SubmitScene);
	}

	void GameManager::ResetPlayerData(const std::string& GameScene, const std::string& SubmitScene)
	{
		m_Data.m_Lives = LIVESONSTART;
		m_Data.m_Score = 0;
		m_GameSceneName = GameScene;
		m_HighScoreSubmitSceneName = SubmitScene;
	}
};