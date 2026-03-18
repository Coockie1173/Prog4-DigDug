#include "ScoreComponent.h"
#include "GameObject.h"
#include "../EventManager.h"
#include "../SteamManager.h"
#include "../Hash.h"

dae::ScoreComponent::ScoreComponent(GameObject* Parent)
	: Component(Parent), m_score(0)
{
}

void dae::ScoreComponent::Update()
{
}

void dae::ScoreComponent::LateUpdate()
{
}

void dae::ScoreComponent::Init()
{
}

void dae::ScoreComponent::AddScore(int points)
{
	m_score += points;
	std::string playerName = GetParent()->GetName();
	unsigned int eventHash = 0;

	if (playerName == "Player1")
		eventHash = make_sdbm_hash("Player1ScoreChanged");
	else if (playerName == "Player2")
		eventHash = make_sdbm_hash("Player2ScoreChanged");

	std::any scoreData = m_score;
	EventManager::GetInstance().Publish(eventHash, scoreData);

	// Unlock achievement when score reaches 100
	if (m_score >= 100)
	{
		SteamManager::GetInstance().UnlockAchievement("ACH_WIN_ONE_GAME");
	}
}
