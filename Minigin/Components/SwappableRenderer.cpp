#include "SwappableRenderer.h"

dae::SwappableRenderer::SwappableRenderer(GameObject* Parent) : RenderComponent(Parent)
{
}

void dae::SwappableRenderer::Render() const
{
	RenderAssignedTexture();
}

void dae::SwappableRenderer::Update()
{
}

void dae::SwappableRenderer::LateUpdate()
{
}

void dae::SwappableRenderer::Init()
{
}

bool dae::SwappableRenderer::Deserialize(const std::map<std::string, std::string>& properties, std::string& errorMessage)
{
	//for now this renderer assumes the textures are added externally
	return true;
}
