#ifndef _PLAYERCONTROLLER_COMPONENT_H_
#define _PLAYERCONTROLLER_COMPONENT_H_

#include "Component.h"
#include <memory>
#include <Command.h>
#include <vector>
#include <Texture2D.h>
#include <glm/glm.hpp>
#include <StateHelper.h>
#include <Hash.h>

namespace dae
{
	class SwappableRenderComponent;
	class ObjectMoveComponent;
	class TerrainGridComponent;
	class PlayerState;
	class MoveIntentCommand;
	class EnemyComponent;

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
		GameObject* GetOwner() const { return GetParent(); }
		TerrainGridComponent* GetTerrainGrid() const;

		void OnPlayerMove();
		void OnPlayerAttack();
		void OnPlayerEndAttack();
		void OnPlayerDig();
		void OnPlayerEndDig();
		void AddMoveIntent(const glm::vec2& direction);
		glm::vec2 GetMoveIntent() const noexcept { return m_MoveIntent; }
		glm::vec2 GetFacingVector() const;
		void ClearMoveIntent() noexcept { m_MoveIntent = glm::vec2{}; }
		bool GetIsAttacking() const noexcept { return m_PlayerAttacking; }
		bool GetIsDigging() const noexcept { return m_PlayerDigging; }
		void AlertPumperDone();

		template<typename T>
		T* GetState() { return m_pStatePool->Get<T>(); }

		void OnPlayerHitEnemy(EnemyComponent* enemy);

		static constexpr unsigned int PLAYERREADYHASH = make_sdbm_hash("PlayerReady");
		static constexpr unsigned int PLAYERDEADHASH = make_sdbm_hash("PlayerDied");

	private:
		enum class FacingDirection
		{
			Right,
			Left,
			Up,
			Down
		};

		// EXPOSE_TO_EDITOR("Input Scheme", "Defines the input scheme as follows: horizontal|vertical")
		std::string m_inputScheme{};
		// EXPOSE_TO_EDITOR("Attack Action Name", "The name of the action to use for attacking")
		std::string m_attackActionName{};
		// EXPOSE_TO_EDITOR("Cancel Action Name", "The name of the action to use for canceling your attack")
		std::string m_cancelActionName{};

		dae::SwappableRenderComponent* m_pRenderComponent{ nullptr };
		dae::ObjectMoveComponent* m_pMoveComponent{ nullptr };
		
		std::vector<std::shared_ptr<dae::Command>> m_Commands{};
		glm::vec2 m_MoveIntent{};
		glm::vec2 m_LastMoveIntent{};
		float m_WalkTimer{};
		bool m_PlayerAttacking{false};
		bool m_PlayerDigging{false};
		float m_AttackCooldownRemaining{ 0.0f };
		float m_DigCooldownRemaining{ 0.0f };
		FacingDirection m_FacingDirection{ FacingDirection::Right };

		std::unique_ptr<StatePool<PlayerState>> m_pStatePool{nullptr};
		PlayerState* m_pCurrentState{ nullptr };

		glm::vec2 ResolveCardinalMoveIntent() const;
		void UpdateFacingFromMoveIntent(const glm::vec2& moveIntent);
		void ApplyFacingToRenderComponent() const;
		glm::ivec2 m_PreviousCell{};
		bool m_HasPreviousCell{ false };
		void PlayerDeath();
		bool m_PlayerDied{ false };

		mutable TerrainGridComponent* m_pCachedTerrainGrid{ nullptr };
	};
}

#endif