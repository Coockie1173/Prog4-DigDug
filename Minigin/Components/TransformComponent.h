#ifndef _TRANSFORMCOMPONENT_H_
#define _TRANSFORMCOMPONENT_H_

#include <glm/glm.hpp>
#include "Component.h"

class TransformComponent final : public Component
{
public:
	TransformComponent(GameObject* Parent);

	const glm::vec2& GetPosition() { return m_position; }
	void SetPosition(float x, float y);
	void SetPosition(const glm::vec2& position);
	void Update() override;
	void LateUpdate() override;
	void Init() override;
private:
	glm::vec2 m_position;
};

#endif // !_TRANSFORMCOMPONENT_H_
