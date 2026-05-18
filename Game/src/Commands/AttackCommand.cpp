#include <Commands/AttackCommand.h>

bool dae::AttackCommand::Execute()
{
	if (auto playerController = GetAttachedComponent())
	{
		if (m_StartAttack)
		{
			playerController->OnPlayerAttack();
		}
		return true;
	}
	return false;
}