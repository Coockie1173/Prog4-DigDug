#include <SDL3/SDL.h>
#include <backends/imgui_impl_sdl3.h>
#include "InputManager.h"
#include "Debugger.h"
#include "Commands/Command.h"
#include <algorithm>
#include <unordered_map>
#include <vector>

namespace dae
{
	struct InputManager::InputBinding
	{
		enum class DeviceType { Gamepad, Keyboard };

		DeviceType deviceType{};
		std::string action;

		int controllerId{};
		GamepadButton button{};

		SDL_Keycode keycode{};
		bool wasPressed{};
	};

	struct InputManager::ActionBinding
	{
		std::string action;
		InputType inputType{};
		std::vector<std::weak_ptr<Command>> commands;
	};
}

dae::InputManager::InputManager() = default;
dae::InputManager::~InputManager() = default;

bool dae::InputManager::ProcessInput()
{
	m_ControllerManager.Update();

	const bool* keyboardState = SDL_GetKeyboardState(nullptr);
	auto executeAction = [this](const std::string& action, InputType inputType)
	{
		auto bindingIt = std::find_if(m_ActionBindings.begin(), m_ActionBindings.end(),
			[&action, inputType](const std::unique_ptr<ActionBinding>& binding)
			{
				return binding->action == action && binding->inputType == inputType;
			});

		if (bindingIt == m_ActionBindings.end())
		{
			return;
		}

		auto& cmds = (*bindingIt)->commands;
		cmds.erase(
			std::remove_if(cmds.begin(), cmds.end(),
				[](auto& weakCmd)
				{
					if (auto cmd = weakCmd.lock())
					{
						cmd->Execute();
						return false;
					}
					return true;
				}),
			cmds.end());
	};

	for (const auto& binding : m_Bindings)
	{
		bool pressed = false;
		bool down = false;
		bool released = false;

		if (binding->deviceType == InputBinding::DeviceType::Gamepad)
		{
			pressed = m_ControllerManager.IsPressed(binding->controllerId, binding->button);
			down = m_ControllerManager.IsDown(binding->controllerId, binding->button);
			released = m_ControllerManager.IsReleased(binding->controllerId, binding->button);
		}
		else
		{
			bool isCurrentlyPressed = keyboardState[SDL_GetScancodeFromKey(binding->keycode, nullptr)];
			pressed = isCurrentlyPressed && !binding->wasPressed;
			down = isCurrentlyPressed;
			released = !isCurrentlyPressed && binding->wasPressed;

			binding->wasPressed = isCurrentlyPressed;
		}

		if (pressed)
		{
			executeAction(binding->action, InputType::Pressed);
		}

		if (down)
		{
			executeAction(binding->action, InputType::Down);
		}

		if (released)
		{
			executeAction(binding->action, InputType::Released);
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

void dae::InputManager::BindButton(int ControllerId, GamepadButton Button,
	const std::string& Action)
{
	auto binding = std::make_unique<InputBinding>();
	binding->deviceType = InputBinding::DeviceType::Gamepad;
	binding->controllerId = ControllerId;
	binding->button = Button;
	binding->action = Action;

	m_Bindings.push_back(std::move(binding));
}

void dae::InputManager::BindKey(SDL_Keycode Keycode,
	const std::string& Action)
{
	auto binding = std::make_unique<InputBinding>();
	binding->deviceType = InputBinding::DeviceType::Keyboard;
	binding->keycode = Keycode;
	binding->action = Action;
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

void dae::InputManager::BindActionToCommand(const std::string& Action, std::shared_ptr<Command> Command, InputType InputType)
{
	auto bindingIt = std::find_if(m_ActionBindings.begin(), m_ActionBindings.end(),
		[&Action, InputType](const std::unique_ptr<ActionBinding>& binding)
		{
			return binding->action == Action && binding->inputType == InputType;
		});

	if (bindingIt == m_ActionBindings.end())
	{
		auto binding = std::make_unique<ActionBinding>();
		binding->action = Action;
		binding->inputType = InputType;
		binding->commands.push_back(Command);
		m_ActionBindings.push_back(std::move(binding));
	}
	else
	{
		(*bindingIt)->commands.push_back(Command);
	}
}
