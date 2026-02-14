#ifndef _TEXTRENDERCOMPONENT_H_
#define _TEXTRENDERCOMPONENT_H_

#include <string>
#include <memory>
#include <SDL3_ttf/SDL_ttf.h>

#include "RenderComponent.h"

namespace dae
{
	class Font;
	class Texture2D;
}

using namespace dae;

class TextRenderComponent : public RenderComponent
{
public:
	TextRenderComponent(GameObject* Parent, const std::string& Text, const SDL_Color& color, std::shared_ptr<Font> font);

	void SetText(const std::string& text) { m_text = text; m_needsUpdate = true; };
	void SetColor(const SDL_Color& color) { m_color = color; m_needsUpdate = true; };

	void Render() const override;
	void Update() override;
	void LateUpdate() override;
	void Init() override;
private:
	bool m_needsUpdate{};
	std::string m_text{};
	SDL_Color m_color{ 255,255,255,255 };
	std::shared_ptr<Font> m_font{};
	std::shared_ptr<Texture2D> m_textTexture{};
};

#endif // ! _TEXTRENDERCOMPONENT_H_
