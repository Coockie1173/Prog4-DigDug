#pragma once
#include "Component.h"
#include "../EventManager.h"
#include <string>

namespace dae
{
	class HealthComponent;

	class LivesDisplayComponent final : public Component
	{
	public:
		LivesDisplayComponent(GameObject* Parent, const std::string& playerName, HealthComponent* health, int maxLives = 3);
		~LivesDisplayComponent();

		void Update() override;
		void LateUpdate() override;
		void Init() override;

	private:
		void OnPlayerDied(unsigned int eventHash, const std::any& data);
		void UpdateDisplay();

		int m_remainingLives;
		int m_maxLives;
		std::string m_playerName;
		HealthComponent* m_health;
		EventManager::EventId m_healthChangeSubscriptionId;
		EventManager::EventId m_deathSubscriptionId;
	};
}
