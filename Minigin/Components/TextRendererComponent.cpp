#include "TextRendererComponent.h"
#include "Font.h"
#include "Renderer.h"
#include <stdexcept>
#include "GameObject.h"
#include "Texture2D.h"

namespace dae
{
	TextRenderComponent::TextRenderComponent(dae::GameObject* Parent, const std::string& Text, const SDL_Color& color, std::shared_ptr<dae::Font> font)
		: m_needsUpdate(true), m_text(Text), m_color(color), m_font(std::move(font)), m_textTexture(nullptr), RenderComponent(Parent)
	{

	}

	void TextRenderComponent::Render() const
	{
		if (m_textTexture != nullptr)
		{
			const auto& pos =  GetParent()->GetPosition();
			dae::Renderer::GetInstance().RenderTexture(*m_textTexture, pos.x, pos.y);
		}
	}

	void TextRenderComponent::Update()
	{
		if (m_needsUpdate)
		{
			const auto surf = TTF_RenderText_Blended(m_font->GetFont(), m_text.c_str(), m_text.length(), m_color);
			if (surf == nullptr)
			{
				throw std::runtime_error(std::string("Render text failed: ") + SDL_GetError());
			}
			auto texture = SDL_CreateTextureFromSurface(dae::Renderer::GetInstance().GetSDLRenderer(), surf);
			if (texture == nullptr)
			{
				throw std::runtime_error(std::string("Create text texture from surface failed: ") + SDL_GetError());
			}
			SDL_DestroySurface(surf);
			m_textTexture = std::make_unique<dae::Texture2D>(texture);
			m_needsUpdate = false;
		}
	}

	void TextRenderComponent::LateUpdate()
	{
	}

	void TextRenderComponent::Init()
	{
	}
}
