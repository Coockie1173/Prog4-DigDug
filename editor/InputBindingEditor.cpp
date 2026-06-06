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

    if (ImGui::BeginTabBar("BindingTabs"))
    {
        if (ImGui::BeginTabItem("Button Bindings"))
        {
            RenderAddBindingSection();
            ImGui::Separator();
            RenderBindingsList();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Axis Bindings"))
        {
            RenderAddAxisBindingSection();
            ImGui::Separator();
            RenderAxisBindingsList();
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

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
            m_capturingAxisKey = false;
            m_listeningForButton = false;
            m_selectedKeyCode = SDLK_UNKNOWN;
        }

        if (m_selectedKeyCode != SDLK_UNKNOWN)
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
                if (m_selectedKeyCode != SDLK_UNKNOWN)
                {
                    binding.keyCode = m_selectedKeyCode;
                    m_bindings[m_actionNameBuffer] = binding;
                    memset(m_actionNameBuffer, 0, sizeof(m_actionNameBuffer));
                    m_selectedKeyCode = SDLK_UNKNOWN;
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

void InputBindingEditor::RenderAddAxisBindingSection()
{
    ImGui::Text("Add New Axis Binding");

    ImGui::InputText("Action Name##axis", m_actionNameBuffer, sizeof(m_actionNameBuffer));

    ImGui::RadioButton("Keyboard##axisdevice", reinterpret_cast<int*>(&m_selectedDeviceType), static_cast<int>(InputDeviceType::Keyboard));
    ImGui::SameLine();
    ImGui::RadioButton("Gamepad##axisdevice", reinterpret_cast<int*>(&m_selectedDeviceType), static_cast<int>(InputDeviceType::Gamepad));

    if (m_selectedDeviceType == InputDeviceType::Gamepad)
    {
        ImGui::SliderInt("Gamepad Index##axis", &m_selectedGamepadIndex, 0, 3);

        int axisIndex = GetAxisIndex(m_selectedAxis);
        if (ImGui::Combo("Axis##gamepad", &axisIndex,
            "Left Stick X\0Left Stick Y\0Right Stick X\0Right Stick Y\0Left Trigger\0Right Trigger\0"))
        {
            m_selectedAxis = GetAxisFromIndex(axisIndex);
        }

        ImGui::SliderFloat("Deadzone##axis", &m_selectedDeadzone, 0.f, 0.9f, "%.2f");
    }
    else
    {
        if (ImGui::Button("Listen##positive"))
        {
            m_listeningForKey = true;
            m_listeningForButton = false;
            m_capturingAxisKey = true;
            m_selectedPositiveKey = SDLK_UNKNOWN;
        }
        ImGui::SameLine();
        ImGui::Text("Positive (+): %s",
            m_selectedPositiveKey != SDLK_UNKNOWN
            ? GetKeyName(m_selectedPositiveKey).c_str()
            : "None");

        if (ImGui::Button("Listen##negative"))
        {
            m_listeningForKey = true;
            m_listeningForButton = true;
            m_capturingAxisKey = true;
            m_selectedNegativeKey = SDLK_UNKNOWN;
        }
        ImGui::SameLine();
        ImGui::Text("Negative (-): %s",
            m_selectedNegativeKey != SDLK_UNKNOWN
            ? GetKeyName(m_selectedNegativeKey).c_str()
            : "None");

        RenderKeyListener();
    }

    if (ImGui::Button("Add Axis Binding"))
    {
        if (strlen(m_actionNameBuffer) > 0)
        {
            AxisBinding binding;
            binding.actionName = m_actionNameBuffer;
            binding.deviceType = m_selectedDeviceType;

            if (m_selectedDeviceType == InputDeviceType::Gamepad)
            {
                binding.gamepadIndex = m_selectedGamepadIndex;
                binding.axis = m_selectedAxis;
                binding.deadzone = m_selectedDeadzone;
                m_axisBindings[m_actionNameBuffer] = binding;
                memset(m_actionNameBuffer, 0, sizeof(m_actionNameBuffer));
            }
            else
            {
                if (m_selectedPositiveKey != SDLK_UNKNOWN &&
                    m_selectedNegativeKey != SDLK_UNKNOWN)
                {
                    binding.positiveKey = m_selectedPositiveKey;
                    binding.negativeKey = m_selectedNegativeKey;
                    m_axisBindings[m_actionNameBuffer] = binding;
                    memset(m_actionNameBuffer, 0, sizeof(m_actionNameBuffer));
                    m_selectedPositiveKey = SDLK_UNKNOWN;
                    m_selectedNegativeKey = SDLK_UNKNOWN;
                }
                else
                {
                    ImGui::SameLine();
                    ImGui::TextColored({ 1, 0.3f, 0.3f, 1 }, "Set both keys first");
                }
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

        std::string toRemove;

        for (auto& [key, binding] : m_bindings)
        {
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%s", key.c_str());

            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%s", binding.deviceType == InputDeviceType::Keyboard ? "Keyboard" : "Gamepad");

            ImGui::TableSetColumnIndex(2);
            if (binding.deviceType == InputDeviceType::Keyboard)
                ImGui::Text("%s", GetKeyName(binding.keyCode).c_str());
            else
                ImGui::Text("GP%d: %s", binding.gamepadIndex, GetButtonName(binding.gamepadButton).c_str());

            ImGui::TableSetColumnIndex(3);
            std::string btnId = "Remove##" + key;
            if (ImGui::Button(btnId.c_str(), ImVec2(-1, 0)))
                toRemove = key;
        }

        ImGui::EndTable();

        if (!toRemove.empty())
            m_bindings.erase(toRemove);
    }
}

void InputBindingEditor::RenderAxisBindingsList()
{
    ImGui::Text("Current Axis Bindings");

    if (ImGui::BeginTable("AxisBindingsTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
    {
        ImGui::TableSetupColumn("Action Name");
        ImGui::TableSetupColumn("Type");
        ImGui::TableSetupColumn("Binding");
        ImGui::TableSetupColumn("Action");
        ImGui::TableHeadersRow();

        std::string toRemove;

        for (auto& [key, binding] : m_axisBindings)
        {
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%s", key.c_str());

            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%s", binding.deviceType == InputDeviceType::Keyboard ? "Keyboard" : "Gamepad");

            ImGui::TableSetColumnIndex(2);
            if (binding.deviceType == InputDeviceType::Gamepad)
                ImGui::Text("GP%d: %s (dz:%.2f)", binding.gamepadIndex,
                    GetAxisName(binding.axis).c_str(), binding.deadzone);
            else
                ImGui::Text("+%s / -%s",
                    GetKeyName(binding.positiveKey).c_str(),
                    GetKeyName(binding.negativeKey).c_str());

            ImGui::TableSetColumnIndex(3);
            std::string btnId = "Remove##axis_" + key;
            if (ImGui::Button(btnId.c_str(), ImVec2(-1, 0)))
                toRemove = key;
        }

        ImGui::EndTable();

        if (!toRemove.empty())
            m_axisBindings.erase(toRemove);
    }
}

void InputBindingEditor::RenderKeyListener()
{
    if (m_listeningForKey)
        ImGui::OpenPopup("Listen for Key");

    if (ImGui::BeginPopupModal("Listen for Key", &m_listeningForKey,
        ImGuiWindowFlags_AlwaysAutoResize))
    {
        if (m_capturingAxisKey)
            ImGui::Text("Press any key for %s...",
                m_listeningForButton ? "Negative (-)" : "Positive (+)");
        else
            ImGui::Text("Press any key...");

        if (ImGui::Button("Cancel##cancelkey"))
        {
            m_listeningForKey = false;
            m_listeningForButton = false;
            m_capturingAxisKey = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void InputBindingEditor::RenderButtonListener()
{
    
}

void InputBindingEditor::SetCapturedKey(SDL_Keycode keyCode)
{
    if (!m_listeningForKey)
        return;

    if (m_capturingAxisKey)
    {
        if (m_listeningForButton)
            m_selectedNegativeKey = keyCode;
        else
            m_selectedPositiveKey = keyCode;
        m_capturingAxisKey = false;
    }
    else
    {
        m_selectedKeyCode = keyCode;
    }

    m_listeningForKey = false;
    m_listeningForButton = false;
    ImGui::CloseCurrentPopup();
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
    case dae::GamepadButton::A:             return "A";
    case dae::GamepadButton::B:             return "B";
    case dae::GamepadButton::X:             return "X";
    case dae::GamepadButton::Y:             return "Y";
    case dae::GamepadButton::DPadUp:        return "DPad Up";
    case dae::GamepadButton::DPadDown:      return "DPad Down";
    case dae::GamepadButton::DPadLeft:      return "DPad Left";
    case dae::GamepadButton::DPadRight:     return "DPad Right";
    case dae::GamepadButton::Start:         return "Start";
    case dae::GamepadButton::Back:          return "Back";
    case dae::GamepadButton::LeftShoulder:  return "Left Shoulder";
    case dae::GamepadButton::RightShoulder: return "Right Shoulder";
    default:                                return "Unknown";
    }
}

int InputBindingEditor::GetButtonIndex(dae::GamepadButton button) const
{
    return static_cast<int>(button);
}

dae::GamepadButton InputBindingEditor::GetButtonFromIndex(int index) const
{
    if (index >= 0 && index <= 11)
        return static_cast<dae::GamepadButton>(index);
    return dae::GamepadButton::A;
}

std::string InputBindingEditor::GetAxisName(dae::GamepadAxis axis) const
{
    switch (axis)
    {
    case dae::GamepadAxis::LeftStickX:   return "Left Stick X";
    case dae::GamepadAxis::LeftStickY:   return "Left Stick Y";
    case dae::GamepadAxis::RightStickX:  return "Right Stick X";
    case dae::GamepadAxis::RightStickY:  return "Right Stick Y";
    case dae::GamepadAxis::LeftTrigger:  return "Left Trigger";
    case dae::GamepadAxis::RightTrigger: return "Right Trigger";
    default:                             return "Unknown";
    }
}

int InputBindingEditor::GetAxisIndex(dae::GamepadAxis axis) const
{
    return static_cast<int>(axis);
}

dae::GamepadAxis InputBindingEditor::GetAxisFromIndex(int index) const
{
    if (index >= 0 && index <= 5)
        return static_cast<dae::GamepadAxis>(index);
    return dae::GamepadAxis::LeftStickX;
}