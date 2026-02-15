#ifndef _FPS_COUNTER_COMPONENT_H_
#define _FPS_COUNTER_COMPONENT_H_

#include "Component.h"

class TextRenderComponent;
class FPSCounterComponent : public Component
{
public:
	FPSCounterComponent(dae::GameObject* Parent);
	void Update() override;
	void LateUpdate() override;
	void Init() override;
private:
	TextRenderComponent* m_parentRenderer;
};

#endif