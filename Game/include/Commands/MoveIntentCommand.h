#ifndef _MOVEINTENT_COMMAND_H_
#define _MOVEINTENT_COMMAND_H_

#include <ComponentCommand.h>
#include <Components/PlayerControllerComponent.h>
#include <glm/glm.hpp>

namespace dae
{
	class MoveIntentCommand final : public ComponentCommand<PlayerControllerComponent>
	{
	public:
		MoveIntentCommand(PlayerControllerComponent* pComponent, const glm::vec2& direction) noexcept
			: ComponentCommand(pComponent), m_Direction(direction)
		{
		}

		bool Execute() override;

	private:
		glm::vec2 m_Direction{};
	};
}

#endif