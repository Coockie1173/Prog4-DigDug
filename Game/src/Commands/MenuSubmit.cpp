#include <Commands/MenuSubmit.h>

bool dae::MenuSubmitCommand::Execute()
{
	if (auto MenuController = GetAttachedComponent())
	{
		MenuController->Submit();
		return true;
	}
	return false;
}