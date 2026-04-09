#include "ScoreComponent.h"
#include "GameObject.h"
#include "../EventManager.h"
#ifdef USE_STEAMWORKS
#include "../SteamManager.h"
#endif
#include "../Hash.h"
#include <map>

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

bool dae::ScoreComponent::Deserialize(const std::map<std::string, std::string>& properties, std::string& errorMessage)
{
	(void)properties;
	(void)errorMessage;
	return true;
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
}
