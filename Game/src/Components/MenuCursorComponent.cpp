#include <Components/MenuCursorComponent.h>
#include <GameObject.h>
#include <Commands/MainMenuMovementCommand.h>
#include <Commands/MenuSubmit.h>
#include <InputManager.h>
#include <ComponentFactoryRegistry.h>
#include <ComponentTypeMap.h>
#include <Timing.h>
#include <EventManager.h>
#include <SceneManager.h>
#include <GameManager.h>

namespace
{
	const bool MenuCursorFactory = dae::RegisterComponentFactoryFor<dae::MenuCursorComponent>(dae::HASH_MenuCursorComponent);
}

namespace dae
{
	void MenuCursorComponent::Init()
	{
		m_OrigY = this->GetParent()->GetWorldPosition().y;

		auto cmd = std::make_shared<MainMenuMovementCommand>(this);
		InputManager::GetInstance().BindAxisActionToCommand(m_VerticalInputName, cmd);
		InputManager::GetInstance().BindAxisActionToCommand(m_VerticalInputNameController, cmd);
		m_Commands.push_back(std::move(cmd));
		auto submitcmd = std::make_shared<MenuSubmitCommand>(this);
		InputManager::GetInstance().BindActionToCommand(m_SubmitButton, submitcmd, InputManager::InputType::Pressed);
		InputManager::GetInstance().BindActionToCommand(m_SubmitButtonController, submitcmd, InputManager::InputType::Pressed);
		m_Commands.push_back(std::move(submitcmd));
	}

	void MenuCursorComponent::Update()
	{
		m_TimerBetweenMoves -= Timing::GetInstance().GetDeltaTime();
	}

	void MenuCursorComponent::LateUpdate()
	{

	}

	bool MenuCursorComponent::Deserialize(const std::map<std::string, std::string>& properties, std::string& errorMessage)
	{
		if (!GetRequiredProperty(properties, "GameSceneName", m_GameSceneName, errorMessage, "MenuCursorComponent")) return false;
		if (!GetRequiredProperty(properties, "HighSceneName", m_HighSceneName, errorMessage, "MenuCursorComponent")) return false;
		if (!GetRequiredProperty(properties, "VerticalInputName", m_VerticalInputName, errorMessage, "MenuCursorComponent")) return false;
		if (!GetRequiredProperty(properties, "VerticalInputNameController", m_VerticalInputNameController, errorMessage, "MenuCursorComponent")) return false;
		if (!GetRequiredProperty(properties, "SubmitButton", m_SubmitButton, errorMessage, "MenuCursorComponent")) return false;
		if (!GetRequiredProperty(properties, "SubmitButtonController", m_SubmitButtonController, errorMessage, "MenuCursorComponent")) return false;
		if (!GetRequiredProperty(properties, "HighSubSceneName", m_HighSubSceneName, errorMessage, "MenuCursorComponent")) return false;

		m_GameSceneName = "Data/" + m_GameSceneName;
		m_HighSceneName = "Data/" + m_HighSceneName;
		m_HighSubSceneName = "Data/" + m_HighSubSceneName;

		return true;
	}

	void MenuCursorComponent::MoveCursor(bool Up)
	{
		if (m_TimerBetweenMoves > 0)
		{
			return;
		}
		int Addition{ Up ? -1 : 1 };
	
		m_CurrentSelection += Addition;
		if (m_CurrentSelection < 0)
		{
			m_CurrentSelection = MAXSELECTIONCOUNT - 1;
		}
		else if (m_CurrentSelection == MAXSELECTIONCOUNT)
		{
			m_CurrentSelection = 0;
		}

		auto pos = this->GetParent()->GetLocalPosition();
		pos.y = m_OrigY + (OFFSETBETWEENITEMS * m_CurrentSelection);
		this->GetParent()->SetLocalPosition(pos);

		m_TimerBetweenMoves = 0.2f;
	}

	void MenuCursorComponent::Submit()
	{
		switch (m_CurrentSelection)
		{
		case 0:
			GameManager::GetInstance().StartSoloGame(m_GameSceneName, m_HighSubSceneName, m_HighSceneName);
			EventManager::GetInstance().Publish(SceneManager::CHANGELEVELHASH, m_GameSceneName);
			break;
		case 1:
			GameManager::GetInstance().StartMPGame(m_GameSceneName, m_HighSubSceneName, m_HighSceneName);
			EventManager::GetInstance().Publish(SceneManager::CHANGELEVELHASH, m_GameSceneName);
			break;
		case 2:
			GameManager::GetInstance().StartVSGame(m_GameSceneName, m_HighSubSceneName, m_HighSceneName);
			EventManager::GetInstance().Publish(SceneManager::CHANGELEVELHASH, m_GameSceneName);
			break;
		case 3:
			EventManager::GetInstance().Publish(SceneManager::CHANGELEVELHASH, m_HighSceneName);
			break;
		default:
			break;
		}
	}
};