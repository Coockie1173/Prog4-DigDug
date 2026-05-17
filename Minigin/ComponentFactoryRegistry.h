#pragma once

#include <cstdint>
#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>

#include <GameObject.h>

namespace dae
{
	class Component;

	using ComponentFactory = std::function<Component*(GameObject*)>;

	bool RegisterComponentFactory(uint32_t componentTypeHash, ComponentFactory factory);
	bool UnregisterComponentFactory(uint32_t componentTypeHash);
	void ClearComponentFactories();
	Component* CreateComponentFromHash(uint32_t componentTypeHash, GameObject* gameObject);

	template<typename T, typename... Args>
	bool RegisterComponentFactoryFor(uint32_t componentTypeHash, Args&&... args)
	{
		static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");

		return RegisterComponentFactory(
			componentTypeHash,
			[defaults = std::make_tuple(std::forward<Args>(args)...)](GameObject* gameObject) -> Component*
			{
				return std::apply(
					[gameObject](const auto&... unpacked) -> Component*
					{
						return gameObject->AddComponent<T>(unpacked...);
					},
					defaults
				);
			}
		);
	}
}