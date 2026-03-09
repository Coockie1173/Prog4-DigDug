#pragma once
#include "Singleton.h"
#include "Controller/ControllerManager.h"

namespace dae
{
	class InputManager final : public Singleton<InputManager>
	{
		ControllerManager m_ControllerManager;
	public:
		bool ProcessInput();
	};

}
