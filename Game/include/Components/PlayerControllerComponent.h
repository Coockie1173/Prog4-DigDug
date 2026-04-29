#ifndef _PLAYERCONTROLLER_COMPONENT_H_
#define _PLAYERCONTROLLER_COMPONENT_H_

#include "Component.h"

class PlayerControllerComponent final : public dae::Component
{
	public:
	PlayerControllerComponent(dae::GameObject* Parent);
	void Update() override;
	void LateUpdate() override;
	void Init() override;
	bool Deserialize(const std::map<std::string, std::string>& properties, std::string& errorMessage) override;
};

#endif