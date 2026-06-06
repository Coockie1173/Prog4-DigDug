#ifndef _INPUTBINDINGDATA_H_
#define _INPUTBINDINGDATA_H_

#include <string>
#include <SDL3/SDL.h>
#include <GamepadButton.h>

namespace dae
{
    /// <summary>
    /// Enumeration for input device types
    /// </summary>
    enum class InputDeviceType
    {
        Keyboard,
        Gamepad
    };

    /// <summary>
    /// Shared input binding data structure used for serialization/deserialization
    /// </summary>
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

    struct AxisBinding
    {
        InputDeviceType deviceType{ InputDeviceType::Keyboard };
        std::string actionName;

        int gamepadIndex{ 0 };
        GamepadAxis axis{ GamepadAxis::LeftStickX };
        float deadzone{ 0.15f };

        SDL_Keycode positiveKey{ SDLK_UNKNOWN };
        SDL_Keycode negativeKey{ SDLK_UNKNOWN };
    };
}
#endif
