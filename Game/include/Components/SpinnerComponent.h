#ifndef _SPINNERCOMPONENT_H_
#define _SPINNERCOMPONENT_H_

#include "Component.h"
#include <glm/glm.hpp>
#include <map>
#include <string>

namespace dae
{
	class SpinnerComponent final : public Component
	{
	public:
		SpinnerComponent(GameObject* Parent, bool SpinClockwise, float SpinRadius, float SpinSpeed, glm::vec2 SpinPoint);
		void Update() override;
		void LateUpdate() override;
		void Init() override;
		bool Deserialize(const std::map<std::string, std::string>& properties, std::string& errorMessage) override;
	private:
		bool m_spinClockwise{};
		float m_spinRadius{};
		float m_spinSpeed{};
		float m_spinAngle{};
		glm::vec2 m_spinPoint{};
	};
}

#endif