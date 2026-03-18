#pragma once
#include "ComponentCommand.h"
#include "Components/HealthComponent.h"

namespace dae
{
	class PlayerDeathCommand final : public ComponentCommand<HealthComponent>
	{
	public:
		PlayerDeathCommand(HealthComponent* pHealthComponent) noexcept
			: ComponentCommand(pHealthComponent)
		{
		}

		bool Execute() override;
	};
}
