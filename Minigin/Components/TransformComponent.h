#ifndef _TRANSFORMCOMPONENT_H_
#define _TRANSFORMCOMPONENT_H_

#include <glm/glm.hpp>
#include "Component.h"

class TransformComponent final : Component
{
public:
	TransformComponent(GameObject& Parent);

	const glm::vec2& GetPosition() { return m_position; }
	void SetPosition(float x, float y, float z = 0);
	void SetPosition(const glm::vec2& position);
private:
	glm::vec2 m_position;
};

#endif // !_TRANSFORMCOMPONENT_H_
