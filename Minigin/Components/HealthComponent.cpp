#include "HealthComponent.h"
#include "GameObject.h"
#include "../EventManager.h"
#include "../Hash.h"

dae::HealthComponent::HealthComponent(GameObject* Parent, int maxHealth)
	: Component(Parent), m_currentHealth(maxHealth), m_maxHealth(maxHealth)
{
}

void dae::HealthComponent::Update()
{
}

void dae::HealthComponent::LateUpdate()
{
}

void dae::HealthComponent::Init()
{
}

void dae::HealthComponent::TakeDamage(int damage)
{
	if (!IsAlive()) return;

	m_currentHealth -= damage;
	if (m_currentHealth < 0) m_currentHealth = 0;

	// Always publish health changed event for display update
	std::string playerName = GetParent()->GetName();
	unsigned int healthChangeHash = 0;
	if (playerName == "Player1")
		healthChangeHash = make_sdbm_hash("Player1HealthChanged");
	else if (playerName == "Player2")
		healthChangeHash = make_sdbm_hash("Player2HealthChanged");

	std::any healthData = m_currentHealth;
	EventManager::GetInstance().Publish(healthChangeHash, healthData);

	// Publish death event only when health reaches 0
	if (!IsAlive())
	{
		unsigned int eventHash = 0;
		if (playerName == "Player1")
			eventHash = make_sdbm_hash("Player1Died");
		else if (playerName == "Player2")
			eventHash = make_sdbm_hash("Player2Died");

		std::any playerData = playerName;
		EventManager::GetInstance().Publish(eventHash, playerData);
	}
}

void dae::HealthComponent::Respawn()
{
	m_currentHealth = m_maxHealth;
}
