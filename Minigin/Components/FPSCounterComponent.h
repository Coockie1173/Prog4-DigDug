#ifndef _FPS_COUNTER_COMPONENT_H_
#define _FPS_COUNTER_COMPONENT_H_

#include "Component.h"

namespace dae
{
	class TextRenderComponent;
	class FPSCounterComponent : public Component
	{
	public:
		FPSCounterComponent(GameObject* Parent);
		void Update() override;
		void LateUpdate() override;
		void Init() override;
	private:
		TextRenderComponent* m_parentRenderer;
		float m_UpdateTimer{};
	};
};

#endif