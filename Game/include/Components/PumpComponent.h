#ifndef _PUMPCOMPONENT_H_
#define _PUMPCOMPONENT_H_

#include "Component.h"
#include <glm/glm.hpp>
#include <memory>
#include <functional>

namespace dae
{
	class GameObject;

	//don't expose, since this gets auto-added to our gameobject
	// NOEXPOSE
	class PumpComponent final : public Component
	{
	public:
		PumpComponent(GameObject* Parent);
		void Configure(glm::vec2 direction, float travelDistance, float speed, std::function<void()> onFinished, std::function<void(GameObject*)> onHitEnemy);
		void Update() override;
		void LateUpdate() override;
		void Init() override;
		bool Deserialize(const std::map<std::string, std::string>& properties, std::string& errorMessage) override;

	private:
		void FinishPump();

		glm::vec2 m_Direction{ 0.0f, 0.0f };
		float m_TravelDistance{ 0.0f };
		float m_Speed{ 0.0f };
		float m_TravelledDistance{ 0.0f };
		std::function<void()> m_OnFinished{};
		std::function<void(GameObject*)> m_OnHitEnemy{};
		bool m_IsConfigured{ false };
		bool m_IsFinished{ false };
	};
}

#endif