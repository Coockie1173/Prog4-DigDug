#include <Commands/MainMenuMovementCommand.h>
#include <Components/MenuCursorComponent.h>

namespace dae
{
	bool MainMenuMovementCommand::Execute()
	{
		auto axis = GetAxisValue();
		if (axis != 0)
		{
			GetAttachedComponent()->MoveCursor(axis > 0);
		}
		return true;
	}
};