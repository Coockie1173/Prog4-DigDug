#include <Commands/MoveIntentCommand.h>
#include <Components/PlayerControllerComponent.h>

bool dae::MoveIntentCommand::Execute()
{
    float V = this->GetAxisValue();
    if (V == 0)
    {
        return true;
    }

    float Mult = std::signbit(V) ? -1.f : 1.f;

    if (auto playerController = GetAttachedComponent())
    {
        playerController->AddMoveIntent(m_Direction * Mult);
        return true;
    }

    return false;
}