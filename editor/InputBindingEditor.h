#pragma once

#include <string>
#include <map>
#include <SDL3/SDL.h>
#include "../Minigin/Controller/GamepadButton.h"

enum class InputDeviceType
{
	Keyboard,
	Gamepad
};

struct InputBinding
{
	InputDeviceType deviceType;
	std::string actionName;
	SDL_Keycode keyCode;
	int gamepadIndex;
	dae::GamepadButton gamepadButton;

	InputBinding()
		: deviceType(InputDeviceType::Keyboard)
		, keyCode(0)
		, gamepadIndex(0)
		, gamepadButton(dae::GamepadButton::A)
	{
	}
};

class InputBindingEditor
{
public:
	InputBindingEditor();

	void Render();

	const std::map<std::string, InputBinding>& GetBindings() const { return m_bindings; }
	std::map<std::string, InputBinding>& GetBindings() { return m_bindings; }

	bool IsListeningForKey() const { return m_listeningForKey; }
	void SetCapturedKey(SDL_Keycode keyCode) { m_selectedKeyCode = keyCode; m_listeningForKey = false; }

private:
	std::map<std::string, InputBinding> m_bindings;
	bool m_showWindow = true;

	char m_actionNameBuffer[256] = {};
	InputDeviceType m_selectedDeviceType = InputDeviceType::Keyboard;
	dae::GamepadButton m_selectedGamepadButton = dae::GamepadButton::A;
	int m_selectedGamepadIndex = 0;
	SDL_Keycode m_selectedKeyCode = 0;

	bool m_listeningForKey = false;
	bool m_listeningForButton = false;

	std::string m_selectedBindingKey = "";

	void RenderAddBindingSection();
	void RenderBindingsList();
	void RenderKeyListener();
	void RenderButtonListener();

	std::string GetKeyName(SDL_Keycode keyCode) const;
	std::string GetButtonName(dae::GamepadButton button) const;
	int GetButtonIndex(dae::GamepadButton button) const;
	dae::GamepadButton GetButtonFromIndex(int index) const;
};
