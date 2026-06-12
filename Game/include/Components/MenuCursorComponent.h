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
		void Submit();
	private:
		bool Deserialize(const std::map<std::string, std::string>& properties, std::string& errorMessage) override;

		// EXPOSE_TO_EDITOR("Gamescenename", "Game scene name")
		std::string m_GameSceneName{ "" };
		// EXPOSE_TO_EDITOR("Coopscenename", "Coop Game scene name")
		std::string m_GameSceneNameCoop{ "" };
		// EXPOSE_TO_EDITOR("Highscorescenename", "High score scene name")
		std::string m_HighSceneName{ "" };
		// EXPOSE_TO_EDITOR("HighscoreSubmitscenename", "High score submission scene name")
		std::string m_HighSubSceneName{ "" };

		// EXPOSE_TO_EDITOR("VerticalInput", "VerticalInput")
		std::string m_VerticalInputName{ "" };
		// EXPOSE_TO_EDITOR("VerticalInputC", "VerticalInput Controller")
		std::string m_VerticalInputNameController{ "" };
		// EXPOSE_TO_EDITOR("Submit", "Submit")
		std::string m_SubmitButton{ "" };
		// EXPOSE_TO_EDITOR("SubmitC", "Submit Controller")
		std::string m_SubmitButtonController{ "" };

		float m_OrigY{ 0 };
		int m_CurrentSelection{ 0 };
		float m_TimerBetweenMoves{ 0 };

		static constexpr int MAXSELECTIONCOUNT{ 4 };
		static constexpr float OFFSETBETWEENITEMS{ 25.f };

		std::vector<std::shared_ptr<dae::Command>> m_Commands{};
	};
};

#endif