#include "ControllerManager.h"
#include <Windows.h>
#include <Xinput.h>
#include <array>
#include <memory>
#include <winerror.h>

#pragma comment(lib,"xinput.lib")

namespace dae
{

    class ControllerManager::Impl
    {
    public:

        struct ControllerState
        {
            XINPUT_STATE previousState{};
            XINPUT_STATE currentState{};

            WORD pressed{};
            WORD released{};

            bool connected{};
        };

        std::array<ControllerState, XUSER_MAX_COUNT> controllers{};

        static WORD ConvertButton(GamepadButton button)
        {
            switch (button)
            {
                case GamepadButton::A: return XINPUT_GAMEPAD_A;
                case GamepadButton::B: return XINPUT_GAMEPAD_B;
                case GamepadButton::X: return XINPUT_GAMEPAD_X;
                case GamepadButton::Y: return XINPUT_GAMEPAD_Y;

                case GamepadButton::DPadUp: return XINPUT_GAMEPAD_DPAD_UP;
                case GamepadButton::DPadDown: return XINPUT_GAMEPAD_DPAD_DOWN;
                case GamepadButton::DPadLeft: return XINPUT_GAMEPAD_DPAD_LEFT;
                case GamepadButton::DPadRight: return XINPUT_GAMEPAD_DPAD_RIGHT;

                case GamepadButton::Start: return XINPUT_GAMEPAD_START;
                case GamepadButton::Back: return XINPUT_GAMEPAD_BACK;

                case GamepadButton::LeftShoulder: return XINPUT_GAMEPAD_LEFT_SHOULDER;
                case GamepadButton::RightShoulder: return XINPUT_GAMEPAD_RIGHT_SHOULDER;
            }

            return 0;
        }

        void Update()
        {
            for (DWORD i{}; i < XUSER_MAX_COUNT; ++i)
            {
                auto& c = controllers[i];

                c.previousState = c.currentState;

                DWORD result = XInputGetState(i, &c.currentState);

                if (result == ERROR_SUCCESS)
                {
                    c.connected = true;

                    WORD changes =
                        c.currentState.Gamepad.wButtons ^ c.previousState.Gamepad.wButtons;

                    c.pressed = changes & c.currentState.Gamepad.wButtons;
                    c.released = changes & (~c.currentState.Gamepad.wButtons);
                }
                else
                {
                    c.connected = false;
                    c.currentState = {};
                    c.previousState = {};
                    c.pressed = 0;
                    c.released = 0;
                }
            }
        }

        bool IsPressed(int idx, GamepadButton button) const
        {
            const auto& c = controllers[idx];
            if (!c.connected) return false;

            return c.pressed & ConvertButton(button);
        }

        bool IsDown(int idx, GamepadButton button) const
        {
            const auto& c = controllers[idx];
            if (!c.connected) return false;

            return c.currentState.Gamepad.wButtons & ConvertButton(button);
        }

        bool IsReleased(int idx, GamepadButton button) const
        {
            const auto& c = controllers[idx];
            if (!c.connected) return false;

            return c.released & ConvertButton(button);
        }
    };

    ControllerManager::ControllerManager()
        : m_pImpl(std::make_unique<Impl>())
    {
    }

    ControllerManager::~ControllerManager() = default;

    void ControllerManager::Update()
    {
        m_pImpl->Update();
    }

    bool ControllerManager::IsPressed(int controller, GamepadButton button) const
    {
        return m_pImpl->IsPressed(controller, button);
    }

    bool ControllerManager::IsDown(int controller, GamepadButton button) const
    {
        return m_pImpl->IsDown(controller, button);
    }

    bool ControllerManager::IsReleased(int controller, GamepadButton button) const
    {
        return m_pImpl->IsReleased(controller, button);
    }

}