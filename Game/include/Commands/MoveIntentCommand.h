#ifndef _MOVEINTENT_COMMAND_H_
#define _MOVEINTENT_COMMAND_H_

#include <AxisComponentCommand.h>
#include <Components/PlayerControllerComponent.h>
#include <glm/glm.hpp>

namespace dae
{
	class MoveIntentCommand final : public AxisComponentCommand<PlayerControllerComponent>
	{
	public:
		MoveIntentCommand(PlayerControllerComponent* pComponent, const glm::vec2& direction) noexcept
			: AxisComponentCommand<PlayerControllerComponent>(pComponent),	m_Direction(direction)
		{}

		bool Execute() override;

	private:
		glm::vec2 m_Direction{};
	};
}

#endif