#include <Commands/MoveIntentCommand.h>
#include <Components/PlayerControllerComponent.h>

bool dae::MoveIntentCommand::Execute()
{
    if (auto playerController = GetAttachedComponent())
    {
        playerController->AddMoveIntent(m_Direction);
        return true;
    }

    return false;
}