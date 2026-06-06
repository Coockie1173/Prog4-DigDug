#pragma once

#include <string>
#include <map>
#include <SDL3/SDL.h>
#include <InputBindingData.h>

// Use the shared InputDeviceType and InputBinding from dae namespace
using InputBinding = dae::InputBinding;
using InputDeviceType = dae::InputDeviceType;
using AxisBinding = dae::AxisBinding;

class InputBindingEditor
{
public:
	InputBindingEditor();

	void Render();

	const std::map<std::string, InputBinding>& GetBindings() const { return m_bindings; }
	std::map<std::string, InputBinding>& GetBindings() { return m_bindings; }
	const std::map<std::string, AxisBinding>& GetAxisBindings() const { return m_axisBindings; }
	std::map<std::string, AxisBinding>& GetAxisBindings() { return m_axisBindings; }

	bool IsListeningForKey() const { return m_listeningForKey; }
	void SetCapturedKey(SDL_Keycode keyCode);

private:
	std::map<std::string, InputBinding> m_bindings;
	std::map<std::string, AxisBinding>  m_axisBindings;
	bool m_showWindow = true;

	char m_actionNameBuffer[256] = {};
	InputDeviceType m_selectedDeviceType = InputDeviceType::Keyboard;
	dae::GamepadButton m_selectedGamepadButton = dae::GamepadButton::A;
	int m_selectedGamepadIndex = 0;
	SDL_Keycode m_selectedKeyCode = 0;

	bool m_listeningForKey = false;
	bool m_listeningForButton = false;

	std::string m_selectedBindingKey = "";

	dae::GamepadAxis m_selectedAxis{ dae::GamepadAxis::LeftStickX };
	float m_selectedDeadzone{ 0.15f };
	SDL_Keycode m_selectedPositiveKey{ SDLK_UNKNOWN };
	SDL_Keycode m_selectedNegativeKey{ SDLK_UNKNOWN };
	bool m_capturingAxisKey{ false };

	void RenderAddBindingSection();
	void RenderBindingsList();
	void RenderAddAxisBindingSection();
	void RenderAxisBindingsList();
	void RenderKeyListener();
	void RenderButtonListener();

	std::string GetKeyName(SDL_Keycode keyCode) const;
	std::string GetButtonName(dae::GamepadButton button) const;
	int GetButtonIndex(dae::GamepadButton button) const;
	dae::GamepadButton GetButtonFromIndex(int index) const;
	std::string GetAxisName(dae::GamepadAxis axis) const;
	int GetAxisIndex(dae::GamepadAxis axis) const;
	dae::GamepadAxis GetAxisFromIndex(int index) const;
};
