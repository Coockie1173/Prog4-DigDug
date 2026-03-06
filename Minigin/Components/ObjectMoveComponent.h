#ifndef _OBJECTMOVECOMPONENT_H_
#define _OBJECTMOVECOMPONENT_H_

#include "Component.h"
#include <glm/glm.hpp>

namespace dae
{
	class ObjectMoveComponent final : public Component
	{
	public:
		ObjectMoveComponent(GameObject* Parent);
		void Update() override;
		void LateUpdate() override;
		void Init() override;

		void MoveObject(glm::vec2& direction, float speed);
	private:

	};
}

#endif