#include "TextureRendererComponent.h"
#include "../ResourceManager.h"
#include "../Renderer.h"
#include <glm/glm.hpp>
#include "RenderComponent.h"
#include "../GameObject.h"
#include "Texture2D.h"

namespace dae
{
	TextureRenderComponent::TextureRenderComponent(dae::GameObject* Parent, const std::string& TextureName) : RenderComponent(Parent)
	{
		m_pTexture = dae::ResourceManager::GetInstance().LoadTexture(TextureName);
	}

	void TextureRenderComponent::Render() const
	{
		RenderAssignedTexture();
	}

	void TextureRenderComponent::Update() {}
	void TextureRenderComponent::LateUpdate() {}
	void TextureRenderComponent::Init() {}
}
