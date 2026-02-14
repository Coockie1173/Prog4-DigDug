#include "TextureRendererComponent.h"
#include "../ResourceManager.h"

TextureRenderComponent::TextureRenderComponent(GameObject& Parent, const std::string& TextureName) : RenderComponent(Parent)
{
	m_texture = ResourceManager::GetInstance().LoadTexture(TextureName);
}

void TextureRenderComponent::Render() const
{

}
