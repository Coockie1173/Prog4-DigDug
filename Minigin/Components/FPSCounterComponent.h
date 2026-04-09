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
		bool Deserialize(const std::map<std::string, std::string>& properties, std::string& errorMessage) override;
	private:
		TextRenderComponent* m_parentRenderer;
		// EXPOSE_TO_EDITOR("Update Timer", "Time interval for FPS updates")
		float m_UpdateTimer{};
	};
};

#endif