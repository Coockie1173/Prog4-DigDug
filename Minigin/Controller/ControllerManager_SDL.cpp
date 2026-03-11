#include "ControllerManager.h"
#include <SDL3/SDL.h>
#include <array>
#include <memory>

namespace dae
{

    class ControllerManager::Impl
    {
    public:

        struct ControllerState
        {
            SDL_Gamepad* gamepad{};
            std::array<bool, SDL_GAMEPAD_BUTTON_COUNT> previous{};
            std::array<bool, SDL_GAMEPAD_BUTTON_COUNT> current{};
        };

        std::array<ControllerState, 4> controllers{};

        static SDL_GamepadButton ConvertButton(GamepadButton button)
        {
            switch (button)
            {
                case GamepadButton::A: return SDL_GAMEPAD_BUTTON_SOUTH;
                case GamepadButton::B: return SDL_GAMEPAD_BUTTON_EAST;
                case GamepadButton::X: return SDL_GAMEPAD_BUTTON_WEST;
                case GamepadButton::Y: return SDL_GAMEPAD_BUTTON_NORTH;

                case GamepadButton::DPadUp: return SDL_GAMEPAD_BUTTON_DPAD_UP;
                case GamepadButton::DPadDown: return SDL_GAMEPAD_BUTTON_DPAD_DOWN;
                case GamepadButton::DPadLeft: return SDL_GAMEPAD_BUTTON_DPAD_LEFT;
                case GamepadButton::DPadRight: return SDL_GAMEPAD_BUTTON_DPAD_RIGHT;

                case GamepadButton::Start: return SDL_GAMEPAD_BUTTON_START;
                case GamepadButton::Back: return SDL_GAMEPAD_BUTTON_BACK;

                case GamepadButton::LeftShoulder: return SDL_GAMEPAD_BUTTON_LEFT_SHOULDER;
                case GamepadButton::RightShoulder: return SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER;
            }

            return SDL_GAMEPAD_BUTTON_INVALID;
        }

        Impl()
        {
            int count = 0;
            auto joysticks = SDL_GetJoysticks(&count);

            int controllerIndex = 0;

            for (int i = 0; i < count && controllerIndex < 4; ++i)
            {
                if (SDL_IsGamepad(joysticks[i]))
                {
                    controllers[controllerIndex].gamepad =
                        SDL_OpenGamepad(joysticks[i]);

                    controllerIndex++;
                }
            }

            SDL_free(joysticks);
        }

        ~Impl()
        {
            for (auto& c : controllers)
            {
                if (c.gamepad)
                    SDL_CloseGamepad(c.gamepad);
            }
        }

        void Update()
        {
            for (auto& c : controllers)
            {
                if (!c.gamepad)
                    continue;

                c.previous = c.current;

                for (int b = 0; b < SDL_GAMEPAD_BUTTON_COUNT; ++b)
                {
                    c.current[b] =
                        SDL_GetGamepadButton(c.gamepad,
                            static_cast<SDL_GamepadButton>(b));
                }
            }
        }

        bool IsPressed(int idx, GamepadButton button) const
        {
            const auto& c = controllers[idx];
            auto b = ConvertButton(button);

            return c.current[b] && !c.previous[b];
        }

        bool IsDown(int idx, GamepadButton button) const
        {
            const auto& c = controllers[idx];
            return c.current[ConvertButton(button)];
        }

        bool IsReleased(int idx, GamepadButton button) const
        {
            const auto& c = controllers[idx];
            auto b = ConvertButton(button);

            return !c.current[b] && c.previous[b];
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