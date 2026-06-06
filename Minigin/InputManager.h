#ifndef _INPUTMANAGER_H_
#define _INPUTMANAGER_H_
#include "Singleton.h"
#include "Controller/ControllerManager.h"
#include <GamepadButton.h>
#include <vector>
#include <memory>
#include <string>
#include <cstdint>

namespace dae
{
	class Command;
	class AxisCommand;

	class InputManager final : public Singleton<InputManager>
	{
	public:
		enum class InputType
		{
			Pressed,
			Down,
			Released
		};

	private:
		struct InputBinding;
		struct ActionBinding;
		struct AxisActionBinding;
		struct AxisBinding;

		ControllerManager m_ControllerManager;
		std::vector<std::unique_ptr<InputBinding>> m_Bindings;
		std::vector<std::unique_ptr<ActionBinding>> m_ActionBindings;
		std::vector<std::unique_ptr<AxisBinding>> m_AxisBindings;
		std::vector<std::unique_ptr<AxisActionBinding>> m_AxisActionBindings;

	public:
		bool ProcessInput();

		// Register a button binding without attaching a command. Commands are attached later via BindActionToCommand.
		void BindButton(int ControllerId, GamepadButton Button,	const std::string& Action);

		// Register a key binding without attaching a command. Commands are attached later via BindActionToCommand.
		void BindKey(SDL_Keycode Keycode, const std::string& Action);

		// Bind a command to an action and the input type that should trigger it.
		// Accepts a shared_ptr so the same command can be attached to multiple bindings.
		void BindActionToCommand(const std::string& Action, std::shared_ptr<Command> Command, InputType InputType);
		
		void BindAxis(int controllerId, GamepadAxis axis, const std::string& action, float deadzone = 0.2f);
		void BindAxisKeys(SDL_Keycode positive, SDL_Keycode negative, const std::string& action);
		void BindAxisActionToCommand(const std::string& action, std::shared_ptr<AxisCommand> command);

		void UnbindAction(const std::string& Action);
		void UnbindKeyAction(const std::string& Action);
		void UnbindKeyPadAction(const std::string& Action);
		void ClearAllBindings();

		InputManager();
		~InputManager();

		InputManager(const InputManager&) = delete;
		InputManager(InputManager&&) = delete;
		InputManager& operator=(const InputManager&) = delete;
		InputManager& operator=(InputManager&&) = delete;
	};

}

#endif
