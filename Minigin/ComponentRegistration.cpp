#include <ComponentTypeMap.h>
#include "ComponentRegistration.h"

#include "GameObject.h"

#include "ComponentFactoryRegistry.h"
#include "ResourceManager.h"

#include "Components/FPSCounterComponent.h"
#include "Components/SpinnerComponent.h"
#include "Components/SwappableRenderComponent.h"
#include "Components/TextRendererComponent.h"
#include "Components/TextureRendererComponent.h"

#include <glm/glm.hpp>

namespace dae
{
	void RegisterBuiltinComponentFactories()
	{
		RegisterComponentFactory(
			HASH_TextureRenderComponent,
			[](GameObject* obj) { return obj->AddComponent<TextureRenderComponent>(""); }
		);
		RegisterComponentFactory(
			HASH_TextRenderComponent,
			[](GameObject* obj)
			{
				auto font = ResourceManager::GetInstance().LoadFont("Lingua.otf", 24);
				return obj->AddComponent<TextRenderComponent>("", SDL_Color{ 255, 255, 255, 255 }, font);
			}
		);
		RegisterComponentFactory(
			HASH_FPSCounterComponent,
			[](GameObject* obj) { return obj->AddComponent<FPSCounterComponent>(); }
		);
		RegisterComponentFactory(
			HASH_SpinnerComponent,
			[](GameObject* obj) { return obj->AddComponent<SpinnerComponent>(true, 50.0f, 1.0f, glm::vec2(400.0f, 300.0f)); }
		);
		RegisterComponentFactory(
			HASH_SwappableRenderComponent,
			[](GameObject* obj) { return obj->AddComponent<SwappableRenderComponent>(); }
		);
	}
}