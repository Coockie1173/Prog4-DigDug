#include "TextRendererComponent.h"
#include "Font.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include <stdexcept>
#include "GameObject.h"
#include "Texture2D.h"
#include "Debugger.h"
#include <format>
#include <sstream>

namespace dae
{
	TextRenderComponent::TextRenderComponent(dae::GameObject* Parent, const std::string& Text, const SDL_Color& color, std::shared_ptr<dae::Font> font)
		: RenderComponent(Parent), m_text(Text), m_color(color), m_font(std::move(font))
	{

	}

	void TextRenderComponent::Render() const
	{
		RenderComponent::RenderAssignedTexture();
	}

	void TextRenderComponent::Update()
	{
		if (m_needsUpdate)
		{
			// Load font with the specified size
			std::shared_ptr<dae::Font> sizedFont = m_font;
			if (m_fontSize != 24)  // If size is not the default
			{
				sizedFont = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", m_fontSize);
			}

			const auto surf = TTF_RenderText_Blended(sizedFont->GetFont(), m_text.c_str(), m_text.length(), m_color);
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
			m_pTexture = std::make_shared<dae::Texture2D>(texture);
			m_needsUpdate = false;
		}
	}

	void TextRenderComponent::LateUpdate()
	{
	}

	void TextRenderComponent::Init()
	{
	}

	bool TextRenderComponent::Deserialize(const std::map<std::string, std::string>& properties, std::string& errorMessage)
	{
		auto textIt = properties.find("text");
		auto fontSizeIt = properties.find("fontSize");
		auto colorIt = properties.find("color");

		if (textIt != properties.end() && fontSizeIt != properties.end())
		{
			try
			{
				m_text = textIt->second;
				m_fontSize = static_cast<uint8_t>(std::stoi(fontSizeIt->second));
				m_font = ResourceManager::GetInstance().LoadFont("Lingua.otf", m_fontSize);

				uint8_t r = 255, g = 255, b = 255, a = 255;
				if (colorIt != properties.end() && !colorIt->second.empty())
				{
					std::istringstream colorStream(colorIt->second);
					std::string token;
					int values[4] = {255, 255, 255, 255};
					int valueCount = 0;

					while (valueCount < 4 && std::getline(colorStream, token, ','))
					{
						try
						{
							values[valueCount] = std::stoi(token);
							valueCount++;
						}
						catch (const std::exception&)
						{
							break;
						}
					}

					if (valueCount >= 3)
					{
						r = static_cast<uint8_t>(values[0]);
						g = static_cast<uint8_t>(values[1]);
						b = static_cast<uint8_t>(values[2]);
						if (valueCount >= 4)
						{
							a = static_cast<uint8_t>(values[3]);
						}
					}
					else
					{
						Debugger::GetInstance().LogDebug(std::format(
							"Invalid color format '{}', using white",
							colorIt->second
						));
					}
				}
				else
					Debugger::GetInstance().LogDebug("color empty, using white default");

				m_color = SDL_Color{r, g, b, a};
				m_needsUpdate = true;
				return true;
			}
			catch (const std::exception& e)
			{
				errorMessage = std::format("Failed to deserialize TextRenderComponent: {}", e.what());
				Debugger::GetInstance().LogError(errorMessage);
				return false;
			}
		}
		else
		{
			errorMessage = "TextRenderComponent missing required properties (text, fontSize)";
			Debugger::GetInstance().LogError(errorMessage);
			return false;
		}
	}
}
