#ifndef _GAMEPADBUTTON_H_
#define _GAMEPADBUTTON_H_

namespace dae
{
	enum class GamepadButton
	{
		A,
		B,
		X,
		Y,

		DPadUp,
		DPadDown,
		DPadLeft,
		DPadRight,

		Start,
		Back,

		LeftShoulder,
		RightShoulder
	};

	enum class GamepadAxis
	{
		LeftStickX,
		LeftStickY,

		RightStickX,
		RightStickY,

		LeftTrigger, 
		RightTrigger,
	};
}

#endif
