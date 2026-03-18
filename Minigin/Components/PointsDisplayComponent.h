#pragma once
#include "Component.h"
#include "../EventManager.h"
#include <string>

namespace dae
{
	class PointsDisplayComponent final : public Component
	{
	public:
		PointsDisplayComponent(GameObject* Parent, const std::string& playerName);
		~PointsDisplayComponent();

		void Update() override;
		void LateUpdate() override;
		void Init() override;

	private:
		void OnScoreChanged(unsigned int eventHash, const std::any& data);
		void UpdateDisplay(int score);

		int m_currentScore{0};
		std::string m_playerName;
		EventManager::EventId m_subscriptionId;
	};
}
