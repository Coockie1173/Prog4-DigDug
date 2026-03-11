#pragma once
#include "Singleton.h"
#include "Controller/ControllerManager.h"
#include "Controller/GamepadButton.h"
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

		void BindButton(int ControllerId, GamepadButton Button, InputType InputType, 
			const std::string& Action, std::unique_ptr<Command> Command);

		void BindKey(SDL_Keycode Keycode, InputType InputType,
			const std::string& Action, std::unique_ptr<Command> Command);

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
