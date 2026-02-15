#include "TextureRendererComponent.h"
#include "../ResourceManager.h"
#include "../Renderer.h"
#include <glm/glm.hpp>
#include "RenderComponent.h"
#include "../GameObject.h"
#include "Texture2D.h"

TextureRenderComponent::TextureRenderComponent(dae::GameObject* Parent, const std::string& TextureName) : RenderComponent(Parent)
{
	m_subtype = RenderSubtype::RENDER_TEXTURE2D;
	m_texture = dae::ResourceManager::GetInstance().LoadTexture(TextureName);
}

void TextureRenderComponent::Render() const
{
	glm::vec2 pos =  m_parent->GetPosition();
	dae::Renderer::GetInstance().RenderTexture(*m_texture, pos.x, pos.y);
}

void TextureRenderComponent::Update() {}
void TextureRenderComponent::LateUpdate() {}
void TextureRenderComponent::Init() {}
