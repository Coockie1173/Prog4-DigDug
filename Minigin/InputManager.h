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

		ControllerManager m_ControllerManager;
		std::vector<std::unique_ptr<InputBinding>> m_Bindings;

	public:
		bool ProcessInput();

		// Register a button binding without attaching a command. Commands are attached later via BindActionToCommand.
		void BindButton(int ControllerId, GamepadButton Button, InputType InputType, 
			const std::string& Action);

		// Register a key binding without attaching a command. Commands are attached later via BindActionToCommand.
		void BindKey(SDL_Keycode Keycode, InputType InputType,
			const std::string& Action);

		// Bind a command to all existing bindings that use the given action name.
		// Accepts a shared_ptr so the same command can be attached to multiple bindings.
		void BindActionToCommand(const std::string& Action, std::shared_ptr<Command> Command);

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
