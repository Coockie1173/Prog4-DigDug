#include <Commands/FrameCounterCommand.h>

bool dae::FrameCounterCommand::Execute()
{
	auto moveComp = GetAttachedComponent();
	if (moveComp)
	{
		moveComp->OnPlayerMove();
		return true;
	}
	return false;
}
