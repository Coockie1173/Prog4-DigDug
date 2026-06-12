#include <Commands/CancelCommand.h>

bool dae::CancelCommand::Execute()
{
    if (auto playerController = GetAttachedComponent())
    {
        playerController->OnPlayerCancel();
        return true;
    }
    return false;
}