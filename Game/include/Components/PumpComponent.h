#ifndef _PUMPCOMPONENT_H_
#define _PUMPCOMPONENT_H_

#include "Component.h"
#include <glm/glm.hpp>

namespace dae
{
	class ObjectMoveComponent;
	class PlayerControllerComponent;
	class GameObject;

	//don't expose, since this gets auto-added to our gameobject
	// NOEXPOSE
	class PumpComponent final : public Component
	{
	public:
		PumpComponent(GameObject* Parent);
		void Configure(PlayerControllerComponent* controller, GameObject* tailObject, glm::vec2 direction, float travelDistance, float speed);
		void Update() override;
		void LateUpdate() override;
		void Init() override;
		bool Deserialize(const std::map<std::string, std::string>& properties, std::string& errorMessage) override;

	private:
		void FinishPump();

		PlayerControllerComponent* m_pController{ nullptr };
		GameObject* m_pTailObject{ nullptr };
		ObjectMoveComponent* m_pMoveComponent{ nullptr };
		glm::vec2 m_Direction{ 0.0f, 0.0f };
		float m_TravelDistance{ 0.0f };
		float m_Speed{ 0.0f };
		float m_TravelledDistance{ 0.0f };
		bool m_IsConfigured{ false };
		bool m_IsFinished{ false };
	};
}

#endif