#include <ComponentTypeMap.h>
#include "ComponentRegistration.h"

#include "GameObject.h"

#include "ComponentFactoryRegistry.h"
#include "ResourceManager.h"

#include "Components/FPSCounterComponent.h"
#include "Components/ObjectMoveComponent.h"
#include "Components/ScoreComponent.h"
#include "Components/SpinnerComponent.h"
#include "Components/SwappableRenderComponent.h"
#include "Components/TextRendererComponent.h"
#include "Components/TextureRendererComponent.h"

#include <glm/glm.hpp>
#include <Components/PlayerControllerComponent.h>

namespace dae
{
	void RegisterGameComponentFactories()
	{
		RegisterComponentFactory(
			HASH_ObjectMoveComponent,
			[](GameObject* obj) { return obj->AddComponent<ObjectMoveComponent>(); }
		);
		RegisterComponentFactory(
			HASH_SpinnerComponent,
			[](GameObject* obj) { return obj->AddComponent<SpinnerComponent>(true, 0.0f, 0.0f, glm::vec2(0.0f, 0.0f)); }
		);
		RegisterComponentFactory(
			HASH_PlayerControllerComponent,
			[](GameObject* obj) { return obj->AddComponent<PlayerControllerComponent>(); }
		);
	}
}