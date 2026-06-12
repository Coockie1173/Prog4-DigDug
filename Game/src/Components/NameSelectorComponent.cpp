#include <Components/NameSelectorComponent.h>
#include <Commands/NameSelectorVerticalCommand.h>
#include <Commands/NameSelectorHorizontalCommand.h>
#include <Commands/NameSelectorConfirmCommand.h>
#include <InputManager.h>
#include <ComponentFactoryRegistry.h>
#include <ComponentTypeMap.h>
#include <Timing.h>
#include <Debugger.h>
#include <Font.h>
#include <Renderer.h>
#include <ResourceManager.h>
#include <Texture2D.h>
#include <GameObject.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdexcept>
#include <format>
#include <EventManager.h>
#include <GameManager.h>

namespace
{
    const bool NameSelectorComponentRegistered =
        dae::RegisterComponentFactoryFor<dae::NameSelectorComponent>(dae::HASH_NameSelectorComponent);
}

namespace dae
{
    void NameSelectorComponent::Init()
    {
        m_font = ResourceManager::GetInstance().LoadFont("Lingua.otf", m_fontSize);

        m_letterDirty.fill(true);
        m_cursorDirty = true;

        auto scrollCmd = std::make_shared<NameSelectorVerticalCommand>(this);
        InputManager::GetInstance().BindAxisActionToCommand(m_VerticalInputName, scrollCmd);
        InputManager::GetInstance().BindAxisActionToCommand(m_VerticalInputNameController, scrollCmd);
        m_Commands.push_back(std::move(scrollCmd));

        auto moveCmd = std::make_shared<NameSelectorHorizontalCommand>(this);
        InputManager::GetInstance().BindAxisActionToCommand(m_HorizontalInputName, moveCmd);
        InputManager::GetInstance().BindAxisActionToCommand(m_HorizontalInputNameController, moveCmd);
        m_Commands.push_back(std::move(moveCmd));

        auto confirmCmd = std::make_shared<NameSelectorConfirmCommand>(this);
        InputManager::GetInstance().BindActionToCommand(m_ConfirmButtonName, confirmCmd, InputManager::InputType::Pressed);
        InputManager::GetInstance().BindActionToCommand(m_ConfirmButtonNameController, confirmCmd, InputManager::InputType::Pressed);
        m_Commands.push_back(std::move(confirmCmd));
    }

    void NameSelectorComponent::Update()
    {
        const float dt = Timing::GetInstance().GetDeltaTime();
        if (m_ScrollCooldownTimer > 0.f) m_ScrollCooldownTimer -= dt;
        if (m_MoveCooldownTimer > 0.f) m_MoveCooldownTimer -= dt;

        for (int i = 0; i < NAME_LENGTH; ++i)
        {
            if (m_letterDirty[i])
            {
                RebuildLetterTexture(i);
                m_letterDirty[i] = false;
            }
        }

        if (m_cursorDirty)
        {
            RebuildCursorTexture();
            m_cursorDirty = false;
        }
    }

    void NameSelectorComponent::LateUpdate() {}

    void NameSelectorComponent::Render() const
    {
        for (int i = 0; i < NAME_LENGTH; ++i)
        {
            if (m_letterTextures[i])
            {
                RenderTextureAt(m_letterTextures[i].get(), i * SLOT_SPACING, 0.f);
            }
        }

        if (m_cursorTexture)
        {
            RenderTextureAt(m_cursorTexture.get(), m_ActiveSlot * SLOT_SPACING, CURSOR_OFFSET_Y);
        }
    }

    void NameSelectorComponent::ScrollLetter(int direction)
    {
        if (m_ScrollCooldownTimer > 0.f) return;

        m_LetterIndices[m_ActiveSlot] =
            (m_LetterIndices[m_ActiveSlot] + direction + ALPHABET_SIZE) % ALPHABET_SIZE;

        m_letterDirty[m_ActiveSlot] = true;
        m_ScrollCooldownTimer = SCROLL_COOLDOWN;
    }

    void NameSelectorComponent::MoveCursor(int direction)
    {
        if (m_MoveCooldownTimer > 0.f) return;

        const int next = m_ActiveSlot + direction;
        if (next >= 0 && next < NAME_LENGTH)
        {
            m_letterDirty[m_ActiveSlot] = true;
            m_ActiveSlot = next;
            m_letterDirty[m_ActiveSlot] = true;
        }

        m_MoveCooldownTimer = MOVE_COOLDOWN;
    }

    void NameSelectorComponent::Confirm()
    {
        EventManager::GetInstance().Publish(GameManager::SUBMITSCOREHASH, GetCurrentName());
    }

    std::string NameSelectorComponent::GetCurrentName() const
    {
        std::string name(NAME_LENGTH, ' ');
        for (int i = 0; i < NAME_LENGTH; ++i)
        {
            name[i] = static_cast<char>(FIRST_CHAR + m_LetterIndices[i]);
        }
        return name;
    }

    char NameSelectorComponent::GetSlotChar(int slot) const
    {
        if (slot < 0 || slot >= NAME_LENGTH) return ' ';
        return static_cast<char>(FIRST_CHAR + m_LetterIndices[slot]);
    }

    void NameSelectorComponent::RebuildLetterTexture(int slot)
    {
        const char letter = GetSlotChar(slot);
        const std::string text(1, letter);

        const SDL_Color& color = (slot == m_ActiveSlot) ? m_activeColor : m_letterColor;

        std::shared_ptr<dae::Font> sizedFont = m_font;
        if (m_fontSize != 24)
        {
            sizedFont = ResourceManager::GetInstance().LoadFont("Lingua.otf", m_fontSize);
        }

        SDL_Surface* surf = TTF_RenderText_Blended(sizedFont->GetFont(), text.c_str(), text.length(), color);
        if (!surf)
            throw std::runtime_error(std::format("NameSelectorComponent: TTF_RenderText_Blended failed: {}", SDL_GetError()));

        SDL_Texture* tex = SDL_CreateTextureFromSurface(
            Renderer::GetInstance().GetSDLRenderer(), surf);
        SDL_DestroySurface(surf);
        if (!tex)
            throw std::runtime_error(std::format("NameSelectorComponent: SDL_CreateTextureFromSurface failed: {}", SDL_GetError()));

        m_letterTextures[slot] = std::make_shared<dae::Texture2D>(tex);
    }

    void NameSelectorComponent::RebuildCursorTexture()
    {
        const std::string cursorGlyph{ "^" };

        std::shared_ptr<dae::Font> sizedFont = m_font;
        if (m_fontSize != 24)
        {
            sizedFont = ResourceManager::GetInstance().LoadFont("Lingua.otf", m_fontSize);
        }

        SDL_Surface* surf = TTF_RenderText_Blended(
            sizedFont->GetFont(), cursorGlyph.c_str(), cursorGlyph.length(), m_cursorColor);
        if (!surf)
            throw std::runtime_error(std::format("NameSelectorComponent: cursor TTF_RenderText_Blended failed: {}", SDL_GetError()));

        SDL_Texture* tex = SDL_CreateTextureFromSurface(
            Renderer::GetInstance().GetSDLRenderer(), surf);
        SDL_DestroySurface(surf);
        if (!tex)
            throw std::runtime_error(std::format("NameSelectorComponent: cursor SDL_CreateTextureFromSurface failed: {}", SDL_GetError()));

        m_cursorTexture = std::make_shared<dae::Texture2D>(tex);
    }

    void NameSelectorComponent::RenderTextureAt(const dae::Texture2D* tex, float offsetX, float offsetY) const
    {
        const auto worldPos = GetParent()->GetWorldPosition();

        float w{}, h{};
        SDL_GetTextureSize(tex->GetSDLTexture(), &w, &h);

        const SDL_FRect dst
        {
            worldPos.x + offsetX - w * 0.5f,
            worldPos.y + offsetY,
            w,
            h
        };

        SDL_RenderTexture(Renderer::GetInstance().GetSDLRenderer(), tex->GetSDLTexture(), nullptr, &dst);
    }

    bool NameSelectorComponent::Deserialize(const std::map<std::string, std::string>& properties,
        std::string& errorMessage)
    {
        if (!GetRequiredProperty(properties, "VerticalInputName", m_VerticalInputName, errorMessage, "NameSelectorComponent")) return false;
        if (!GetRequiredProperty(properties, "VerticalInputNameController", m_VerticalInputNameController, errorMessage, "NameSelectorComponent")) return false;
        if (!GetRequiredProperty(properties, "HorizontalInputName", m_HorizontalInputName, errorMessage, "NameSelectorComponent")) return false;
        if (!GetRequiredProperty(properties, "HorizontalInputNameController", m_HorizontalInputNameController, errorMessage, "NameSelectorComponent")) return false;
        if (!GetRequiredProperty(properties, "ConfirmButtonName", m_ConfirmButtonName, errorMessage, "NameSelectorComponent")) return false;
        if (!GetRequiredProperty(properties, "ConfirmButtonNameController", m_ConfirmButtonNameController, errorMessage, "NameSelectorComponent")) return false;

        auto it = properties.find("FontSize");
        if (it != properties.end() && !it->second.empty())
        {
            try { m_fontSize = static_cast<uint8_t>(std::stoi(it->second)); }
            catch (...)
            {
            }
        }

        return true;
    }
}
