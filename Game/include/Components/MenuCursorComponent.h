#ifndef _MENUCURSORCOMPONENT_H_
#define _MENUCURSORCOMPONENT_H_

#include <Component.h>
#include <string>
#include <vector>
#include <memory>
#include <Command.h>

namespace dae
{
	class MenuCursorComponent final : public Component
	{
	public:
		MenuCursorComponent(GameObject* Parent) : Component(Parent) {};

		void Init() override;
		void Update() override;
		void LateUpdate() override;

		void MoveCursor(bool Up);
	private:
		bool Deserialize(const std::map<std::string, std::string>& properties, std::string& errorMessage) override;

		// EXPOSE_TO_EDITOR("Gamescenename", "Game scene name")
		std::string m_GameSceneName{ "" };
		// EXPOSE_TO_EDITOR("Highscorescenename", "Game scene name")
		std::string m_HighSceneName{ "" };

		// EXPOSE_TO_EDITOR("VerticalInput", "VerticalInput")
		std::string m_VerticalInputName{ "" };
		// EXPOSE_TO_EDITOR("VerticalInputC", "VerticalInput Controller")
		std::string m_VerticalInputNameController{ "" };

		float m_OrigY{ 0 };
		int m_CurrentSelection{ 0 };
		float m_TimerBetweenMoves{ 0 };

		static constexpr int MAXSELECTIONCOUNT{ 4 };
		static constexpr float OFFSETBETWEENITEMS{ 25.f };

		std::vector<std::shared_ptr<dae::Command>> m_Commands{};
	};
};

#endif