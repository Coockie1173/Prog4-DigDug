#include "TextureRendererComponent.h"
#include "../ResourceManager.h"
#include "../Renderer.h"
#include <glm/glm.hpp>
#include "RenderComponent.h"
#include "../GameObject.h"
#include "Texture2D.h"
#include "../Debugger.h"
#include <format>

namespace dae
{
	TextureRenderComponent::TextureRenderComponent(dae::GameObject* Parent, const std::string& TextureName) : RenderComponent(Parent), m_textureName(TextureName)
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

	bool TextureRenderComponent::Deserialize(const std::map<std::string, std::string>& properties, std::string& errorMessage)
	{
		auto textureNameIt = properties.find("textureName");
		if (textureNameIt != properties.end())
		{
			m_textureName = textureNameIt->second;
			m_pTexture = dae::ResourceManager::GetInstance().LoadTexture(m_textureName);
			return true;
		}
		else
		{
			errorMessage = std::format("TextureRenderComponent missing '{}' property", "textureName");
			Debugger::GetInstance().LogError(errorMessage);
			return false;
		}
	}
}
