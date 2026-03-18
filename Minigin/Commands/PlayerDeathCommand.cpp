#include "PlayerDeathCommand.h"

bool dae::PlayerDeathCommand::Execute()
{
	GetAttachedComponent()->TakeDamage(1);
	return true;
}
