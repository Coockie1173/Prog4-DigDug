#ifndef _SPINNERCOMPONENT_H_
#define _SPINNERCOMPONENT_H_

#include "Component.h"
#include <glm/glm.hpp>

namespace dae
{
	class SpinnerComonent : public Component
	{
	public:
		SpinnerComonent(GameObject* Parent, bool SpinClockwise, float SpinRadius, float SpinSpeed, glm::vec2 SpinPoint);

		void Update() override;
		void LateUpdate() override;
		void Init() override;

	private:
		// EXPOSE_TO_EDITOR("Spin Clockwise", "Direction of the spin")
		bool m_spinClockwise;
		// EXPOSE_TO_EDITOR("Spin Radius", "Distance from center point")
		float m_spinRadius;
		// EXPOSE_TO_EDITOR("Spin Speed", "How fast to spin")
		float m_spinSpeed;
		// EXPOSE_TO_EDITOR("Spin Point", "Center point of orbit")
		glm::vec2 m_spinPoint;

		float m_spinAngle = 0.0f;
	};
}


#endif