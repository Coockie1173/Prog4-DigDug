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
		m_texture = dae::ResourceManager::GetInstance().LoadTexture(TextureName);
	}

	void TextureRenderComponent::Render() const
	{
		glm::vec2 pos =  GetParent()->GetPosition();
		dae::Renderer::GetInstance().RenderTexture(*m_texture, pos.x, pos.y);
	}

	void TextureRenderComponent::Update() {}
	void TextureRenderComponent::LateUpdate() {}
	void TextureRenderComponent::Init() {}
}
