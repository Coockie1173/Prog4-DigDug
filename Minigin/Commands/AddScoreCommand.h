#pragma once
#include "ComponentCommand.h"
#include "Components/ScoreComponent.h"

namespace dae
{
	class AddScoreCommand final : public ComponentCommand<ScoreComponent>
	{
	public:
		AddScoreCommand(ScoreComponent* pScoreComponent, int points = 10) noexcept
			: ComponentCommand(pScoreComponent), m_points(points)
		{
		}

		void SetPoints(int points) noexcept { m_points = points; }

		bool Execute() override;

	private:
		int m_points;
	};
}
