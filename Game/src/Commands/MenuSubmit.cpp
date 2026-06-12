#include <Commands/MenuSubmit.h>

bool dae::MenuSubmitCommand::Execute()
{
	if (auto playerController = GetAttachedComponent())
	{
		playerController->Submit();
		return true;
	}
	return false;
}