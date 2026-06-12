#ifndef _NAMESELECTORCOMPONENT_H_
#define _NAMESELECTORCOMPONENT_H_

#include <RenderComponent.h>
#include <Texture2D.h>
#include <string>
#include <vector>
#include <array>
#include <memory>
#include <functional>
#include <Command.h>
#include <SDL3/SDL.h>

namespace dae
{
    class Font;

    // Callback: fired when the player confirms their name.
    using NameConfirmedCallback = std::function<void(const std::string&)>;

    class NameSelectorComponent final : public RenderComponent
    {
    public:
        NameSelectorComponent(GameObject* Parent) : RenderComponent(Parent) {}

        void Init() override;
        void Update() override;
        void LateUpdate() override;
        void Render() const override;

        void ScrollLetter(int direction);

        void MoveCursor(int direction);

        void Confirm();

        void SetOnNameConfirmed(NameConfirmedCallback callback) { m_OnNameConfirmed = std::move(callback); }

        std::string GetCurrentName() const;
        int GetActiveSlot()  const { return m_ActiveSlot; }
        char GetSlotChar(int slot) const;

    private:
        bool Deserialize(const std::map<std::string, std::string>& properties, std::string& errorMessage) override;

        void RebuildLetterTexture(int slot);
        void RebuildCursorTexture();

        void RenderTextureAt(const dae::Texture2D* tex, float offsetX, float offsetY) const;

        static constexpr int   NAME_LENGTH = 3;
        static constexpr float SLOT_SPACING = 40.f;   
        static constexpr float CURSOR_OFFSET_Y = 28.f;

        static constexpr float SCROLL_COOLDOWN = 0.12f;
        static constexpr float MOVE_COOLDOWN = 0.15f;

        static constexpr char  FIRST_CHAR = 'A';
        static constexpr char  LAST_CHAR = 'Z';
        static constexpr int   ALPHABET_SIZE = (LAST_CHAR - FIRST_CHAR) + 1;

        std::shared_ptr<dae::Font> m_font{};
        // EXPOSE_TO_EDITOR("FontSize", "Size of the name selector font")
        uint8_t m_fontSize{ 32 };
        SDL_Color m_letterColor{ 255, 255, 255, 255 };
        SDL_Color m_activeColor{ 255, 220,   0, 255 };  
        SDL_Color m_cursorColor{ 255, 220,   0, 255 };

        std::array<std::shared_ptr<dae::Texture2D>, NAME_LENGTH> m_letterTextures{};
        std::array<bool, NAME_LENGTH> m_letterDirty{ true, true, true };

        std::shared_ptr<dae::Texture2D> m_cursorTexture{};
        bool m_cursorDirty{ true };

        int m_ActiveSlot{ 0 };
        int m_LetterIndices[NAME_LENGTH]{ 0, 0, 0 };
        float m_ScrollCooldownTimer{ 0.f };
        float m_MoveCooldownTimer{ 0.f };

        // ---- input action names ----
        // EXPOSE_TO_EDITOR("VerticalInputName", "Scroll letters — keyboard axis")
        std::string m_VerticalInputName{};
        // EXPOSE_TO_EDITOR("VerticalInputNameController", "Scroll letters — controller axis")
        std::string m_VerticalInputNameController{};
        // EXPOSE_TO_EDITOR("HorizontalInputName", "Move slot — keyboard axis")
        std::string m_HorizontalInputName{};
        // EXPOSE_TO_EDITOR("HorizontalInputNameController", "Move slot — controller axis")
        std::string m_HorizontalInputNameController{};
        // EXPOSE_TO_EDITOR("ConfirmButtonName", "Confirm name — keyboard")
        std::string m_ConfirmButtonName{};
        // EXPOSE_TO_EDITOR("ConfirmButtonNameController", "Confirm name — controller")
        std::string m_ConfirmButtonNameController{};

        NameConfirmedCallback m_OnNameConfirmed{};
        std::vector<std::shared_ptr<dae::Command>> m_Commands{};
    };
}

#endif // _NAMESELECTORCOMPONENT_H_
