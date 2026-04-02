#ifndef _TEXTRENDERCOMPONENT_H_
#define _TEXTRENDERCOMPONENT_H_

#include <string>
#include <memory>
#include <SDL3_ttf/SDL_ttf.h>

#include "RenderComponent.h"
#include "Texture2D.h"

namespace dae
{
	class Font;
	class TextRenderComponent final : public RenderComponent
	{
	public:
		TextRenderComponent(dae::GameObject* Parent, const std::string& Text, const SDL_Color& color, std::shared_ptr<dae::Font> font);

		void SetText(const std::string& text) { m_text = text; m_needsUpdate = true; };
		void SetColor(const SDL_Color& color) { m_color = color; m_needsUpdate = true; };
		void SetSize(uint8_t size) { m_fontSize = size; m_needsUpdate = true; };

		void Render() const override;
		void Update() override;
		void LateUpdate() override;
		void Init() override;
	private:
		bool m_needsUpdate{true};
		// EXPOSE_TO_EDITOR("Text", "The text to display")
		std::string m_text{};
		// EXPOSE_TO_EDITOR("Color", "Text color")
		SDL_Color m_color{ 255,255,255,255 };
		// EXPOSE_TO_EDITOR("Font Size", "Size of the text font")
		uint8_t m_fontSize{ 24 };
		//std::unique_ptr<dae::Texture2D> m_textTexture{};
		std::shared_ptr<dae::Font> m_font{};
	};
}

#endif // ! _TEXTRENDERCOMPONENT_H_
