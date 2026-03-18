#pragma once
#include "Component.h"

namespace dae
{
	class HealthComponent final : public Component
	{
	public:
		HealthComponent(GameObject* Parent, int maxHealth = 3);
		~HealthComponent() = default;

		void Update() override;
		void LateUpdate() override;
		void Init() override;

		int GetHealth() const { return m_currentHealth; }
		int GetMaxHealth() const { return m_maxHealth; }

		void TakeDamage(int damage = 1);
		void Respawn();
		bool IsAlive() const { return m_currentHealth > 0; }

	private:
		int m_currentHealth;
		int m_maxHealth;
	};
}
