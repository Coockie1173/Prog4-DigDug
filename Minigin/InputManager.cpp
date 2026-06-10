#include <SDL3/SDL.h>
#include <backends/imgui_impl_sdl3.h>
#include "InputManager.h"
#include "Debugger.h"
#include "Commands/Command.h"
#include "Commands/AxisCommand.h"
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <cmath>

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

	struct InputManager::AxisBinding
	{
		enum class DeviceType { Gamepad, Keyboard };

		DeviceType deviceType{};
		std::string action;
		int controllerId{};
		GamepadAxis axis{}; 
		float deadzone{ 0.2f };

		SDL_Keycode positiveKey{};
		SDL_Keycode negativeKey{};
	};

	struct InputManager::ActionBinding
	{
		std::string action;
		InputType inputType{};
		std::vector<std::weak_ptr<Command>> commands;
	};

	struct InputManager::AxisActionBinding
	{
		std::string action;
		std::vector<std::weak_ptr<AxisCommand>> commands;
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
                return;

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

    auto executeAxisAction = [this](const std::string& action, float value)
        {
            auto bindingIt = std::find_if(m_AxisActionBindings.begin(), m_AxisActionBindings.end(),
                [&action](const std::unique_ptr<AxisActionBinding>& binding)
                {
                    return binding->action == action;
                });

            if (bindingIt == m_AxisActionBindings.end())
                return;

            auto& cmds = (*bindingIt)->commands;
            cmds.erase(
                std::remove_if(cmds.begin(), cmds.end(),
                    [value](auto& weakCmd)
                    {
                        if (auto cmd = weakCmd.lock())
                        {
							cmd->SetAxisValue(value);
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

        if (pressed)  executeAction(binding->action, InputType::Pressed);
        if (down)     executeAction(binding->action, InputType::Down);
        if (released) executeAction(binding->action, InputType::Released);
    }

    for (const auto& binding : m_AxisBindings)
    {
        float value = 0.f;

        if (binding->deviceType == AxisBinding::DeviceType::Gamepad)
        {
            value = m_ControllerManager.GetAxis(binding->controllerId, binding->axis);

            const float dz = binding->deadzone;
            if (std::abs(value) < dz)
            {
                value = 0.f;
            }
            else
            {
                value = (value - std::copysign(dz, value)) / (1.f - dz);
            }
        }
        else
        {
            bool pos = keyboardState[SDL_GetScancodeFromKey(binding->positiveKey, nullptr)];
            bool neg = keyboardState[SDL_GetScancodeFromKey(binding->negativeKey, nullptr)];
            value = (pos ? 1.f : 0.f) + (neg ? -1.f : 0.f);
        }

        executeAxisAction(binding->action, value);
    }

    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        if (e.type == SDL_EVENT_QUIT)
            return false;

        if (e.type == SDL_EVENT_KEY_DOWN)
        {
            switch (e.key.key)
            {
            case SDLK_ESCAPE:
                return false;
            case SDLK_F6:
                Debugger::GetInstance().DeleteAllLogs();
                break;
            }
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

void dae::InputManager::BindAxis(int controllerId, GamepadAxis axis, const std::string& action, float deadzone)
{
	auto binding = std::make_unique<AxisBinding>();
	binding->deviceType = AxisBinding::DeviceType::Gamepad;
	binding->controllerId = controllerId;
	binding->axis = axis;
	binding->action = action;
	binding->deadzone = deadzone;

	m_AxisBindings.push_back(std::move(binding));
}

void dae::InputManager::BindAxisKeys(SDL_Keycode positive, SDL_Keycode negative, const std::string& action)
{
	auto binding = std::make_unique<AxisBinding>();
	binding->deviceType = AxisBinding::DeviceType::Keyboard;
	binding->positiveKey = positive;
	binding->negativeKey = negative;
	binding->action = action;

	m_AxisBindings.push_back(std::move(binding));
}

void dae::InputManager::BindAxisActionToCommand(const std::string& action, std::shared_ptr<AxisCommand> command)
{
	auto bindingIt = std::find_if(m_AxisActionBindings.begin(), m_AxisActionBindings.end(),
		[&action](const std::unique_ptr<AxisActionBinding>& binding)
		{
			return binding->action == action;
		});

	if (bindingIt == m_AxisActionBindings.end())
	{
		auto binding = std::make_unique<AxisActionBinding>();
		binding->action = action;
		binding->commands.push_back(command);
		m_AxisActionBindings.push_back(std::move(binding));
	}
	else
	{
		(*bindingIt)->commands.push_back(command);
	}
}