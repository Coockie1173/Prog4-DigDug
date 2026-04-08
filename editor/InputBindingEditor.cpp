#include "InputBindingEditor.h"
#include <imgui.h>

InputBindingEditor::InputBindingEditor()
{
	memset(m_actionNameBuffer, 0, sizeof(m_actionNameBuffer));
}

void InputBindingEditor::Render()
{
	if (!ImGui::Begin("Input Binding Editor", &m_showWindow))
	{
		ImGui::End();
		return;
	}

	RenderAddBindingSection();
	ImGui::Separator();
	RenderBindingsList();

	ImGui::End();
}

void InputBindingEditor::RenderAddBindingSection()
{
	ImGui::Text("Add New Binding");

	ImGui::InputText("Action Name", m_actionNameBuffer, sizeof(m_actionNameBuffer));

	ImGui::RadioButton("Keyboard##device", reinterpret_cast<int*>(&m_selectedDeviceType), static_cast<int>(InputDeviceType::Keyboard));
	ImGui::SameLine();
	ImGui::RadioButton("Gamepad##device", reinterpret_cast<int*>(&m_selectedDeviceType), static_cast<int>(InputDeviceType::Gamepad));

	if (m_selectedDeviceType == InputDeviceType::Keyboard)
	{
		if (ImGui::Button("Listen for Key##keyboard"))
		{
			m_listeningForKey = true;
			m_selectedKeyCode = 0;
		}

		if (m_selectedKeyCode != 0)
		{
			ImGui::SameLine();
			ImGui::Text("Key: %s", GetKeyName(m_selectedKeyCode).c_str());
		}

		RenderKeyListener();
	}
	else
	{
		ImGui::SliderInt("Gamepad Index", &m_selectedGamepadIndex, 0, 3);

		int buttonIndex = GetButtonIndex(m_selectedGamepadButton);
		if (ImGui::Combo("Button##gamepad", &buttonIndex, 
			"A\0B\0X\0Y\0DPadUp\0DPadDown\0DPadLeft\0DPadRight\0Start\0Back\0LeftShoulder\0RightShoulder\0"))
		{
			m_selectedGamepadButton = GetButtonFromIndex(buttonIndex);
		}

		ImGui::SameLine();
		ImGui::Text("Button: %s", GetButtonName(m_selectedGamepadButton).c_str());
	}

	if (ImGui::Button("Add Binding"))
	{
		if (strlen(m_actionNameBuffer) > 0)
		{
			InputBinding binding;
			binding.actionName = m_actionNameBuffer;
			binding.deviceType = m_selectedDeviceType;

			if (m_selectedDeviceType == InputDeviceType::Keyboard)
			{
				if (m_selectedKeyCode != 0)
				{
					binding.keyCode = m_selectedKeyCode;
					m_bindings[m_actionNameBuffer] = binding;
					memset(m_actionNameBuffer, 0, sizeof(m_actionNameBuffer));
					m_selectedKeyCode = 0;
					m_listeningForKey = false;
				}
			}
			else
			{
				binding.gamepadIndex = m_selectedGamepadIndex;
				binding.gamepadButton = m_selectedGamepadButton;
				m_bindings[m_actionNameBuffer] = binding;
				memset(m_actionNameBuffer, 0, sizeof(m_actionNameBuffer));
			}
		}
	}
}

void InputBindingEditor::RenderBindingsList()
{
	ImGui::Text("Current Bindings");

	if (ImGui::BeginTable("BindingsTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
	{
		ImGui::TableSetupColumn("Action Name");
		ImGui::TableSetupColumn("Type");
		ImGui::TableSetupColumn("Binding");
		ImGui::TableSetupColumn("Action");
		ImGui::TableHeadersRow();

		std::string toRemove = "";
		int buttonIndex = 0;

		for (auto& [key, binding] : m_bindings)
		{
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("%s", key.c_str());

			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%s", binding.deviceType == InputDeviceType::Keyboard ? "Keyboard" : "Gamepad");

			ImGui::TableSetColumnIndex(2);
			if (binding.deviceType == InputDeviceType::Keyboard)
			{
				ImGui::Text("%s", GetKeyName(binding.keyCode).c_str());
			}
			else
			{
				ImGui::Text("GP%d: %s", binding.gamepadIndex, GetButtonName(binding.gamepadButton).c_str());
			}

			ImGui::TableSetColumnIndex(3);
			std::string buttonId = "Remove##" + key;
			if (ImGui::Button(buttonId.c_str(), ImVec2(-1, 0)))
			{
				toRemove = key;
			}
			buttonIndex++;
		}

		ImGui::EndTable();

		if (!toRemove.empty())
		{
			m_bindings.erase(toRemove);
		}
	}
}

void InputBindingEditor::RenderKeyListener()
{
	if (m_listeningForKey)
	{
		ImGui::OpenPopup("Listen for Key");
	}

	if (ImGui::BeginPopupModal("Listen for Key", &m_listeningForKey, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Press any key...");

		if (ImGui::Button("Cancel##cancelkey"))
		{
			m_listeningForKey = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void InputBindingEditor::RenderButtonListener()
{
	// Placeholder for gamepad button listener
}

std::string InputBindingEditor::GetKeyName(SDL_Keycode keyCode) const
{
	const char* name = SDL_GetKeyName(keyCode);
	return name ? std::string(name) : "Unknown";
}

std::string InputBindingEditor::GetButtonName(dae::GamepadButton button) const
{
	switch (button)
	{
		case dae::GamepadButton::A: return "A";
		case dae::GamepadButton::B: return "B";
		case dae::GamepadButton::X: return "X";
		case dae::GamepadButton::Y: return "Y";
		case dae::GamepadButton::DPadUp: return "DPad Up";
		case dae::GamepadButton::DPadDown: return "DPad Down";
		case dae::GamepadButton::DPadLeft: return "DPad Left";
		case dae::GamepadButton::DPadRight: return "DPad Right";
		case dae::GamepadButton::Start: return "Start";
		case dae::GamepadButton::Back: return "Back";
		case dae::GamepadButton::LeftShoulder: return "Left Shoulder";
		case dae::GamepadButton::RightShoulder: return "Right Shoulder";
		default: return "Unknown";
	}
}

int InputBindingEditor::GetButtonIndex(dae::GamepadButton button) const
{
	return static_cast<int>(button);
}

dae::GamepadButton InputBindingEditor::GetButtonFromIndex(int index) const
{
	if (index >= 0 && index <= 11)
	{
		return static_cast<dae::GamepadButton>(index);
	}
	return dae::GamepadButton::A;
}
