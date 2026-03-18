#include "PointsDisplayComponent.h"
#include "TextRendererComponent.h"
#include "GameObject.h"
#include "../Hash.h"
#include <any>

dae::PointsDisplayComponent::PointsDisplayComponent(GameObject* Parent, const std::string& playerName)
	: Component(Parent), m_currentScore(0), m_playerName(playerName)
{
	auto callback = [this](unsigned int, const std::any& data)
	{
		try
		{
			int score = std::any_cast<int>(data);
			this->UpdateDisplay(score);
		}
		catch (const std::bad_any_cast&)
		{
		}
	};

	unsigned int eventHash = 0;
	if (playerName == "Player1")
		eventHash = make_sdbm_hash("Player1ScoreChanged");
	else if (playerName == "Player2")
		eventHash = make_sdbm_hash("Player2ScoreChanged");

	m_subscriptionId = EventManager::GetInstance().Subscribe(eventHash, callback);
}

dae::PointsDisplayComponent::~PointsDisplayComponent()
{
	unsigned int eventHash = 0;
	if (m_playerName == "Player1")
		eventHash = make_sdbm_hash("Player1ScoreChanged");
	else if (m_playerName == "Player2")
		eventHash = make_sdbm_hash("Player2ScoreChanged");

	EventManager::GetInstance().Unsubscribe(eventHash, m_subscriptionId);
}

void dae::PointsDisplayComponent::Update()
{
}

void dae::PointsDisplayComponent::LateUpdate()
{
}

void dae::PointsDisplayComponent::Init()
{
	UpdateDisplay(0);
}

void dae::PointsDisplayComponent::OnScoreChanged(unsigned int, const std::any& data)
{
	try
	{
		int score = std::any_cast<int>(data);
		m_currentScore = score;
		UpdateDisplay(score);
	}
	catch (const std::bad_any_cast&)
	{
	}
}

void dae::PointsDisplayComponent::UpdateDisplay(int score)
{
	std::string displayText = m_playerName + " Score: " + std::to_string(score);

	auto textComponent = GetParent()->GetComponent<TextRenderComponent>();
	if (textComponent)
	{
		textComponent->SetText(displayText);
	}
}
