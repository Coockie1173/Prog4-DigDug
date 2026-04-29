#pragma once

#include <cstdint>
#include <functional>

namespace dae
{
	class Component;
	class GameObject;

	using ComponentFactory = std::function<Component*(GameObject*)>;

	bool RegisterComponentFactory(uint32_t componentTypeHash, ComponentFactory factory);
	bool UnregisterComponentFactory(uint32_t componentTypeHash);
	void ClearComponentFactories();
	Component* CreateComponentFromHash(uint32_t componentTypeHash, GameObject* gameObject);
}