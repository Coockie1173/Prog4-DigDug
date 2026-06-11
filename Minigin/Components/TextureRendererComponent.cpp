#include "TextureRendererComponent.h"
#include "../ResourceManager.h"
#include "../Renderer.h"
#include "../ComponentFactoryRegistry.h"
#include <ComponentTypeMap.h>
#include <glm/glm.hpp>
#include "RenderComponent.h"
#include "../GameObject.h"
#include "Texture2D.h"
#include "../Debugger.h"
#include <format>

namespace
{
	 const bool TextureRenderComponentRegistered = dae::RegisterComponentFactoryFor<dae::TextureRenderComponent>(dae::HASH_TextureRenderComponent);
}

namespace dae
{
	TextureRenderComponent::TextureRenderComponent(dae::GameObject* Parent) : RenderComponent(Parent), m_textureName("")
	{
		
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
		if (!GetRequiredProperty(properties, "textureName", m_textureName, errorMessage, "TextureRenderComponent"))
		{
			Debugger::GetInstance().LogError(errorMessage);
			return false;
		}

		m_pTexture = dae::ResourceManager::GetInstance().LoadTexture(m_textureName);
		return true;
	}
}
