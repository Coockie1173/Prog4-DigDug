#ifndef _HIGHSCORECOMPONENT_H_
#define _HIGHSCORECOMPONENT_H_

#include <memory>
#include <string>

#include <SDL3_ttf/SDL_ttf.h>

#include "RenderComponent.h"
#include "Texture2D.h"
#include <ComponentCommand.h>
#include <vector>
#include <memory>

namespace dae
{
    class Font;
    class HighScoreComponent final : public RenderComponent
    {
    public:
        explicit HighScoreComponent(GameObject* parent) : RenderComponent(parent) {}

        void Render() const override;
        void Update() override;
        void LateUpdate() override;
        void Init() override;

        bool Deserialize(const std::map<std::string, std::string>& properties, std::string& errorMessage) override;

    private:
        void LoadScores();
        void RebuildTexture();
        void RenderTextureAt(const Texture2D* tex, float offsetX, float offsetY) const;

        bool m_TextDirty{ true };

        SDL_Color m_Color{ 255, 255, 255, 255 };

        // EXPOSE_TO_EDITOR("Font Size", "Size of the text font")
        uint8_t m_FontSize{ 24 };

        // EXPOSE_TO_EDITOR("Score File", "Path to the score file")
        std::string m_FilePath{ "./Data/Scores.txt" };

        // EXPOSE_TO_EDITOR("BackButton", "")
        std::string m_BackButton{ "" };

        // EXPOSE_TO_EDITOR("BackButtonC", "Controller")
        std::string m_BackButtonController{ "" };

        std::string m_Text{};

        std::shared_ptr<Font> m_Font{};
        std::shared_ptr<Texture2D> m_Texture{};
        std::vector<std::shared_ptr<dae::Command>> m_Commands{};
    };

    class HighScoreSubmitCommand final : public ComponentCommand<HighScoreComponent>
    {
    public:
        HighScoreSubmitCommand(HighScoreComponent* pComponent) : ComponentCommand(pComponent) {}
        bool Execute() override;
    };

}

#endif