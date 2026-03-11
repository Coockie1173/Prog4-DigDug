#include <SDL3/SDL.h>
#include <backends/imgui_impl_sdl3.h>
#include "InputManager.h"
#include "Debugger.h"
#include "Commands/Command.h"
#include <algorithm>
#include <unordered_map>

namespace dae
{
	struct InputManager::InputBinding
	{
		enum class DeviceType { Gamepad, Keyboard };

		DeviceType deviceType{};
		InputType inputType{};
		std::string action;
		std::unique_ptr<Command> command;

		int controllerId{};
		GamepadButton button{};

		SDL_Keycode keycode{};
		bool wasPressed{};
	};
}

dae::InputManager::InputManager() = default;
dae::InputManager::~InputManager() = default;

bool dae::InputManager::ProcessInput()
{
	m_ControllerManager.Update();

	const bool* keyboardState = SDL_GetKeyboardState(nullptr);

	for (const auto& binding : m_Bindings)
	{
		bool shouldExecute = false;

		if (binding->deviceType == InputBinding::DeviceType::Gamepad)
		{
			switch (binding->inputType)
			{
			case InputType::Pressed:
				shouldExecute = m_ControllerManager.IsPressed(binding->controllerId, binding->button);
				break;
			case InputType::Down:
				shouldExecute = m_ControllerManager.IsDown(binding->controllerId, binding->button);
				break;
			case InputType::Released:
				shouldExecute = m_ControllerManager.IsReleased(binding->controllerId, binding->button);
				break;
			}
		}
		else
		{
			bool isCurrentlyPressed = keyboardState[SDL_GetScancodeFromKey(binding->keycode, nullptr)];

			switch (binding->inputType)
			{
			case InputType::Pressed:
				shouldExecute = isCurrentlyPressed && !binding->wasPressed;
				break;
			case InputType::Down:
				shouldExecute = isCurrentlyPressed;
				break;
			case InputType::Released:
				shouldExecute = !isCurrentlyPressed && binding->wasPressed;
				break;
			}

			binding->wasPressed = isCurrentlyPressed;
		}

		if (shouldExecute && binding->command)
		{
			binding->command->Execute();
		}
	}

	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		if (e.type == SDL_EVENT_QUIT) 
		{
			return false;
		}
		if (e.type == SDL_EVENT_KEY_DOWN) 
		{
			switch (e.key.key)
			{
			case SDLK_ESCAPE:
			{
				return false;
			}
			case SDLK_F6:
			{
				Debugger::GetInstance().DeleteAllLogs();
			}
			}
		}
		if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) 
		{

		}

		ImGui_ImplSDL3_ProcessEvent(&e);
	}

	return true;
}

void dae::InputManager::BindButton(int ControllerId, GamepadButton Button, InputType InputType,
	const std::string& Action, std::unique_ptr<Command> Command)
{
	auto binding = std::make_unique<InputBinding>();
	binding->deviceType = InputBinding::DeviceType::Gamepad;
	binding->controllerId = ControllerId;
	binding->button = Button;
	binding->inputType = InputType;
	binding->action = Action;
	binding->command = std::move(Command);

	m_Bindings.push_back(std::move(binding));
}

void dae::InputManager::BindKey(SDL_Keycode Keycode, InputType InputType,
	const std::string& Action, std::unique_ptr<Command> Command)
{
	auto binding = std::make_unique<InputBinding>();
	binding->deviceType = InputBinding::DeviceType::Keyboard;
	binding->keycode = Keycode;
	binding->inputType = InputType;
	binding->action = Action;
	binding->command = std::move(Command);
	binding->wasPressed = false;

	m_Bindings.push_back(std::move(binding));
}

void dae::InputManager::UnbindAction(const std::string& Action)
{
	m_Bindings.erase(
		std::remove_if(m_Bindings.begin(), m_Bindings.end(),
			[&Action](const std::unique_ptr<InputBinding>& binding)
			{
				return binding->action == Action;
			}),
		m_Bindings.end()
	);
}

void dae::InputManager::UnbindKeyAction(const std::string& Action)
{
	m_Bindings.erase(
		std::remove_if(m_Bindings.begin(), m_Bindings.end(),
			[&Action](const std::unique_ptr<InputBinding>& binding)
			{
				return binding->deviceType == InputBinding::DeviceType::Keyboard && 
					   binding->action == Action;
			}),
		m_Bindings.end()
	);
}

void dae::InputManager::UnbindKeyPadAction(const std::string& Action)
{
	m_Bindings.erase(
		std::remove_if(m_Bindings.begin(), m_Bindings.end(),
			[&Action](const std::unique_ptr<InputBinding>& binding)
			{
				return binding->deviceType == InputBinding::DeviceType::Gamepad && 
					   binding->action == Action;
			}),
		m_Bindings.end()
	);
}

void dae::InputManager::ClearAllBindings()
{
	m_Bindings.clear();
}
