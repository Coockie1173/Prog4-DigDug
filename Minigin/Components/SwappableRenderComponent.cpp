#include "SwappableRenderComponent.h"
#include "../ComponentFactoryRegistry.h"
#include <ComponentTypeMap.h>

namespace
{
	 const bool SwappableRenderComponentRegistered = dae::RegisterComponentFactoryFor<dae::SwappableRenderComponent>(dae::HASH_SwappableRenderComponent);
}

dae::SwappableRenderComponent::SwappableRenderComponent(GameObject* Parent) : RenderComponent(Parent)
{
}

void dae::SwappableRenderComponent::Render() const
{
	RenderAssignedTexture();
}

void dae::SwappableRenderComponent::Update()
{
}

void dae::SwappableRenderComponent::LateUpdate()
{
}

void dae::SwappableRenderComponent::Init()
{
}

bool dae::SwappableRenderComponent::Deserialize(const std::map<std::string, std::string>&, std::string&)
{
	//for now this renderer assumes the textures are added externally
	return true;
}
