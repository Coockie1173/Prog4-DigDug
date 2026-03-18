#include "AddScoreCommand.h"

bool dae::AddScoreCommand::Execute()
{
	GetAttachedComponent()->AddScore(m_points);
	return true;
}
