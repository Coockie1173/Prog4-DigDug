#include "MovementInputCommand.h"

bool dae::MovementInputCommand::Execute()
{
	auto moveComp = GetAttachedComponent();
	if (moveComp)
	{
		moveComp->MoveObject(m_Direction, m_Speed);
		return true;
	}

	return false;
}