#include "ComponentFactoryRegistry.h"

#include "GameObject.h"
#include "Components/Component.h"

#include <map>
#include <utility>

namespace dae
{
	namespace
	{
		std::map<uint32_t, ComponentFactory>& GetComponentFactories()
		{
			static std::map<uint32_t, ComponentFactory> componentFactories{};
			return componentFactories;
		}
	}

	bool RegisterComponentFactory(uint32_t componentTypeHash, ComponentFactory factory)
	{
		if (!factory)
		{
			return false;
		}

		GetComponentFactories().insert_or_assign(componentTypeHash, std::move(factory));
		return true;
	}

	bool UnregisterComponentFactory(uint32_t componentTypeHash)
	{
		return GetComponentFactories().erase(componentTypeHash) > 0;
	}

	void ClearComponentFactories()
	{
		GetComponentFactories().clear();
	}

	Component* CreateComponentFromHash(uint32_t componentTypeHash, GameObject* gameObject)
	{
		auto& componentFactories = GetComponentFactories();
		auto factoryIt = componentFactories.find(componentTypeHash);
		if (factoryIt == componentFactories.end())
		{
			return nullptr;
		}

		return factoryIt->second(gameObject);
	}
}