#ifndef _PLAYERCONTROLLER_COMPONENT_H_
#define _PLAYERCONTROLLER_COMPONENT_H_

#include "Component.h"
#include <memory>
#include <Command.h>
#include <vector>
#include <Texture2D.h>
#include <glm/glm.hpp>

namespace dae
{
	class SwappableRenderComponent;
	class ObjectMoveComponent;
	class PlayerStatePool;
	class PlayerState;
	class MoveIntentCommand;

	class PlayerControllerComponent final : public dae::Component
	{
	public:
		PlayerControllerComponent(dae::GameObject* Parent);
		~PlayerControllerComponent() override;
		void Update() override;
		void LateUpdate() override;
		void Init() override;
		bool Deserialize(const std::map<std::string, std::string>& properties, std::string& errorMessage) override;

		GameObject* GetPlayer() const { return GetParent(); }

		void OnPlayerMove();
		void OnPlayerAttack();
		void OnPlayerEndAttack();
		void AddMoveIntent(const glm::vec2& direction);
		glm::vec2 GetMoveIntent() const noexcept { return m_MoveIntent; }
		void ClearMoveIntent() noexcept { m_MoveIntent = glm::vec2{}; }
		bool GetIsAttacking() const noexcept { return m_PlayerAttacking; }

	private:
		// EXPOSE_TO_EDITOR("Input Scheme", "Defines the input scheme as follows: upaction|downaction|leftaction|rightaction")
		std::string m_inputScheme{};
		// EXPOSE_TO_EDITOR("Attack Action Name", "The name of the action to use for attacking")
		std::string m_attackActionName{};

		dae::SwappableRenderComponent* m_pRenderComponent{ nullptr };
		dae::ObjectMoveComponent* m_pMoveComponent{ nullptr };
		
		std::vector<std::shared_ptr<dae::Command>> m_Commands{};
		glm::vec2 m_MoveIntent{};
		float m_WalkTimer{};
		bool m_PlayerAttacking{false};

		std::unique_ptr<PlayerStatePool> m_pStatePool{};
		PlayerState* m_pCurrentState{ nullptr };
	};
}

#endif