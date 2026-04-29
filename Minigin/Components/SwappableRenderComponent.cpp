#include "SwappableRenderComponent.h"

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
