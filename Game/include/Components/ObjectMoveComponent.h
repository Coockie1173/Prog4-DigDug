#ifndef _OBJECTMOVECOMPONENT_H_
#define _OBJECTMOVECOMPONENT_H_

#include "Component.h"
#include <glm/glm.hpp>
#include <map>
#include <string>

namespace dae
{
	class ObjectMoveComponent final : public Component
	{
	public:
		ObjectMoveComponent(GameObject* Parent);
		void Update() override;
		void LateUpdate() override;
		void Init() override;
		bool Deserialize(const std::map<std::string, std::string>& properties, std::string& errorMessage) override;

		void MoveObject(glm::vec2& direction, float speed);
	private:

	};
}

#endif