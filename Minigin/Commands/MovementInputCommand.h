#ifndef _MOVEMENTINPUTCOMMAND_H_
#define _MOVEMENTINPUTCOMMAND_H_

#include "ComponentCommand.h"
#include "Components/ObjectMoveComponent.h"
#include <glm/glm.hpp>

namespace dae
{
	class MovementInputCommand final : public ComponentCommand<ObjectMoveComponent>
	{
	public:
		void SetDirection(const glm::vec2& direction) noexcept { m_Direction = direction; }
		void SetSpeed(float speed) noexcept { m_Speed = speed; }

		bool Execute() override;
	private:
		glm::vec2 m_Direction;
		float m_Speed;
	};
}

#endif