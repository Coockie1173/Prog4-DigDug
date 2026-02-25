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
		bool m_spinClockwise;
		float m_spinRadius;
		float m_spinSpeed;
		glm::vec2 m_spinPoint;

		float m_spinAngle = 0.0f;
	};
}


#endif