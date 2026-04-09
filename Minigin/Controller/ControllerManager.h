#ifndef _CONTROLLERMANAGER_H_
#define _CONTROLLERMANAGER_H_

#include <memory>
#include <GamepadButton.h>

namespace dae
{
	class ControllerManager
	{
	public:
		ControllerManager();
		~ControllerManager();

		void Update();

		bool IsPressed(int controller, GamepadButton button) const;
		bool IsDown(int controller, GamepadButton button) const;
		bool IsReleased(int controller, GamepadButton button) const;

	private:
		class Impl;
		std::unique_ptr<Impl> m_pImpl;
	};
}

#endif