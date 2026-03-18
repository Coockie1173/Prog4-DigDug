#include "LivesDisplayComponent.h"
#include "TextRendererComponent.h"
#include "HealthComponent.h"
#include "GameObject.h"
#include "../Hash.h"

dae::LivesDisplayComponent::LivesDisplayComponent(GameObject* Parent, const std::string& playerName, HealthComponent* health, int maxLives)
	: Component(Parent), m_remainingLives(maxLives), m_maxLives(maxLives), m_playerName(playerName), m_health(health)
{
	// Subscribe to health changes
	auto healthCallback = [this](unsigned int, const std::any&)
	{
		this->UpdateDisplay();
	};

	unsigned int healthChangeHash = 0;
	if (playerName == "Player1")
		healthChangeHash = make_sdbm_hash("Player1HealthChanged");
	else if (playerName == "Player2")
		healthChangeHash = make_sdbm_hash("Player2HealthChanged");

	m_healthChangeSubscriptionId = EventManager::GetInstance().Subscribe(healthChangeHash, healthCallback);

	// Subscribe to death events
	auto deathCallback = [this](unsigned int, const std::any&)
	{
		this->OnPlayerDied(0, std::any());
	};

	unsigned int eventHash = 0;
	if (playerName == "Player1")
		eventHash = make_sdbm_hash("Player1Died");
	else if (playerName == "Player2")
		eventHash = make_sdbm_hash("Player2Died");

	m_deathSubscriptionId = EventManager::GetInstance().Subscribe(eventHash, deathCallback);
}

dae::LivesDisplayComponent::~LivesDisplayComponent()
{
	unsigned int healthChangeHash = 0;
	if (m_playerName == "Player1")
		healthChangeHash = make_sdbm_hash("Player1HealthChanged");
	else if (m_playerName == "Player2")
		healthChangeHash = make_sdbm_hash("Player2HealthChanged");

	EventManager::GetInstance().Unsubscribe(healthChangeHash, m_healthChangeSubscriptionId);

	unsigned int deathHash = 0;
	if (m_playerName == "Player1")
		deathHash = make_sdbm_hash("Player1Died");
	else if (m_playerName == "Player2")
		deathHash = make_sdbm_hash("Player2Died");

	EventManager::GetInstance().Unsubscribe(deathHash, m_deathSubscriptionId);
}

void dae::LivesDisplayComponent::Update()
{
}

void dae::LivesDisplayComponent::LateUpdate()
{
}

void dae::LivesDisplayComponent::Init()
{
	UpdateDisplay();
}

void dae::LivesDisplayComponent::OnPlayerDied(unsigned int, const std::any&)
{
	if (m_remainingLives > 0)
	{
		m_remainingLives--;

		// Respawn first, then update display
		if (m_remainingLives > 0 && m_health)
		{
			m_health->Respawn();
		}

		UpdateDisplay();
	}
}

void dae::LivesDisplayComponent::UpdateDisplay()
{
	std::string healthStr;
	if (m_health)
	{
		healthStr = std::to_string(m_health->GetHealth()) + "/" + std::to_string(m_health->GetMaxHealth());
	}
	else
	{
		healthStr = "0/3";
	}

	std::string displayText = m_playerName + " Lives: " + std::to_string(m_remainingLives) + "/" + std::to_string(m_maxLives) + 
		" Health: " + healthStr;

	auto textComponent = GetParent()->GetComponent<TextRenderComponent>();
	if (textComponent)
	{
		textComponent->SetText(displayText);
	}
}
