#ifndef _PLAYERCONTROLLER_COMPONENT_H_
#define _PLAYERCONTROLLER_COMPONENT_H_

#include "Component.h"
#include <memory>
#include <Command.h>
#include <vector>
#include <Texture2D.h>

namespace dae
{
	class SwappableRenderComponent;
	class ObjectMoveComponent;

	class PlayerControllerComponent final : public dae::Component
	{
	public:
		PlayerControllerComponent(dae::GameObject* Parent);
		void Update() override;
		void LateUpdate() override;
		void Init() override;
		bool Deserialize(const std::map<std::string, std::string>& properties, std::string& errorMessage) override;

		void OnPlayerMove();
	private:
		// EXPOSE_TO_EDITOR("Input Scheme", "Defines the input scheme as follows: upaction|downaction|leftaction|rightaction")
		std::string m_inputScheme{};

		//THIS WILL BE REFACTORED OUT v
		// EXPOSE_TO_EDITOR("Idle Frame Name", "The name of the frame to use when not walking")
		std::string m_IdleFrameName{};
		std::shared_ptr<dae::Texture2D> m_IdleFrame{ nullptr };

		// EXPOSE_TO_EDITOR("Walk Frame Name", "The name of the frame to use when walking")
		std::string m_WalkFrameName{};
		std::shared_ptr<dae::Texture2D> m_WalkFrame{ nullptr };

		// EXPOSE_TO_EDITOR("Attack frame", "The name of the frame to use when attacking")
		std::string m_AttackFrameName{};
		std::shared_ptr<dae::Texture2D> m_AttackFrame{ nullptr };
		//THIS WILL BE REFACTORED OUT ^

		dae::SwappableRenderComponent* m_pRenderComponent{ nullptr };
		dae::ObjectMoveComponent* m_pMoveComponent{ nullptr };
		
		//I will fix this later, but this works for now
		std::vector<std::shared_ptr<dae::Command>> m_Commands{ nullptr };
		float m_WalkTimer{};
	};
}

#endif