#pragma once
#include "Component.h"

namespace dae
{
	class ScoreComponent final : public Component
	{
	public:
		ScoreComponent(GameObject* Parent);
		~ScoreComponent() = default;

		void Update() override;
		void LateUpdate() override;
		void Init() override;

		int GetScore() const { return m_score; }
		void AddScore(int points);

	private:
		int m_score{0};
	};
}
