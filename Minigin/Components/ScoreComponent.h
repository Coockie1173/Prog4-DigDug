#pragma once
#include "Component.h"
#include <map>
#include <string>

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
		bool Deserialize(const std::map<std::string, std::string>& properties, std::string& errorMessage) override;

		int GetScore() const { return m_score; }
		void AddScore(int points);

	private:
		// EXPOSE_TO_EDITOR("Score", "Current score value")
		int m_score{0};
	};
}
