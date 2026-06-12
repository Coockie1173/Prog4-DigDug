#include <Components/HighScoreComponent.h>

#include <ComponentTypeMap.h>
#include "../ComponentFactoryRegistry.h"

#include "Font.h"
#include "Renderer.h"
#include <ResourceManager.h>
#include <Texture2D.h>
#include <Debugger.h>
#include <SDL3/SDL.h>
#include <fstream>
#include <sstream>
#include <format>
#include <filesystem>
#include <ComponentTypeMap.h>
#include <format>
#include <sstream>
#include <stdexcept>

namespace
{
	const bool HighScoreComponentRegistered = dae::RegisterComponentFactoryFor<dae::HighScoreComponent>(dae::HASH_HighScoreComponent);
}

namespace dae
{

	void HighScoreComponent::Render() const
	{
		RenderComponent::RenderAssignedTexture();
	}

	void HighScoreComponent::Update()
	{
		if (!m_TextDirty)
			return;

		std::shared_ptr<Font> sizedFont = m_Font;

		sizedFont = ResourceManager::GetInstance().LoadFont("Lingua.otf", m_FontSize);

		const auto surf = TTF_RenderText_Blended_Wrapped(m_Font->GetFont(), m_Text.c_str(), m_Text.length(), m_Color, 0);

		if (surf == nullptr)
		{
			throw std::runtime_error(std::string("Render text failed: ") + SDL_GetError());
		}

		auto texture = SDL_CreateTextureFromSurface(Renderer::GetInstance().GetSDLRenderer(), surf);

		if (texture == nullptr)
		{
			SDL_DestroySurface(surf);

			throw std::runtime_error(std::string("Create text texture from surface failed: ") + SDL_GetError());
		}

		SDL_DestroySurface(surf);

		m_pTexture = std::make_shared<Texture2D>(texture);
		m_TextDirty = false;
	}

	void HighScoreComponent::LateUpdate()
	{}

	void HighScoreComponent::Init()
	{
		LoadScores();
	}

	void HighScoreComponent::LoadScores()
	{
		std::ifstream input{ std::filesystem::path{ m_FilePath } };

		if (!input.is_open())
		{
			Debugger::GetInstance().LogError(std::format("Failed to open score file '{}'", m_FilePath));

			m_Text = "No Scores";
			m_TextDirty = true;
			return;
		}

		std::ostringstream stream;

		int rank{ 1 };

		std::string line;
		while (std::getline(input, line))
		{
			if (line.empty()) continue;

			std::istringstream ss(line);
			std::string name;
			int score{};

			if (ss >> name >> score)
			{
				stream << std::format("{}. {} - {}", rank++, name, score);
				stream << '\n';
			}
		}

		m_Text = stream.str();
		m_TextDirty = true;
	}

	bool HighScoreComponent::Deserialize(const std::map<std::string, std::string>& properties, std::string& errorMessage)
	{
		std::string ColBuf{};
		std::string FontSizeBuf{};
		if (!GetRequiredProperty(properties, "Color", ColBuf, errorMessage, "HighScoreComponent")) return false;
		if (!GetRequiredProperty(properties, "FontSize", FontSizeBuf, errorMessage, "HighScoreComponent")) return false;
		if (!GetRequiredProperty(properties, "FilePath", m_FilePath, errorMessage, "HighScoreComponent")) return false;

		m_FontSize = static_cast<uint8_t>(std::stoi(FontSizeBuf));
		m_Font = ResourceManager::GetInstance().LoadFont("Lingua.otf", m_FontSize);

		return true;
	}
}