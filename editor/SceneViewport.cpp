#include "SceneViewport.h"
#include "EditorScene.h"
#include <cmath>
#include <filesystem>
#include "gencomponents/ComponentRegisterMaster.h"
#include "ResourceManager.h"
#include "Texture2D.h"
#include "Font.h"

SceneViewport::SceneViewport(SDL_Renderer* renderer, int width, int height)
    : m_renderer(renderer), m_width(width), m_height(height)
{
}

SceneViewport::~SceneViewport() = default;

void SceneViewport::SetSize(int width, int height)
{
    m_width = width;
    m_height = height;
}

glm::vec2 SceneViewport::ScreenToWorld(glm::vec2 screenPos) const
{
    return screenPos / SCALE;
}

glm::vec2 SceneViewport::WorldToScreen(glm::vec2 worldPos) const
{
    return glm::vec2(worldPos.x * SCALE, worldPos.y * SCALE);
}

void SceneViewport::Render(const std::vector<std::unique_ptr<dae::GameObject_Barebones>>& objects,
                          dae::GameObject_Barebones* selectedObject)
{
    // Draw background
    SDL_SetRenderDrawColor(m_renderer, 30, 30, 30, 255);
    SDL_RenderFillRect(m_renderer, nullptr);

    // Draw grid
    SDL_SetRenderDrawColor(m_renderer, 60, 60, 60, 100);
    const int gridSpacingInt = 32;
    for (int x = 0; x < m_width; x += gridSpacingInt)
    {
        SDL_RenderLine(m_renderer, static_cast<float>(x), 0.0f, static_cast<float>(x), static_cast<float>(m_height));
    }
    for (int y = 0; y < m_height; y += gridSpacingInt)
    {
        SDL_RenderLine(m_renderer, 0.0f, static_cast<float>(y), static_cast<float>(m_width), static_cast<float>(y));
    }

    // Draw axes
    SDL_SetRenderDrawColor(m_renderer, 100, 100, 100, 150);
    SDL_RenderLine(m_renderer, 0.0f, 0.0f, static_cast<float>(m_width), 0.0f); // X axis (top edge, Y=0)
    SDL_RenderLine(m_renderer, 0.0f, 0.0f, 0.0f, static_cast<float>(m_height));  // Y axis (left edge, X=0)

    // Render GameObjects
    for (const auto& obj : objects)
    {
        glm::vec2 worldPos = obj->GetWorldPosition();
        glm::vec2 screenPos = WorldToScreen(worldPos);
        int dotX = static_cast<int>(screenPos.x);
        int dotY = static_cast<int>(screenPos.y);

        // Draw parent connection line
        if (obj->GetParent())
        {
            glm::vec2 parentWorldPos = obj->GetParent()->GetWorldPosition();
            glm::vec2 parentScreenPos = WorldToScreen(parentWorldPos);
            float parentX = parentScreenPos.x;
            float parentY = parentScreenPos.y;

            SDL_SetRenderDrawColor(m_renderer, 100, 100, 100, 150);
            SDL_RenderLine(m_renderer, parentX, parentY, static_cast<float>(dotX), static_cast<float>(dotY));
        }

        // Draw dot
        bool isSelected = (obj.get() == selectedObject);
        if (isSelected)
        {
            SDL_SetRenderDrawColor(m_renderer, 0, 200, 255, 255);
        }
        else
        {
            SDL_SetRenderDrawColor(m_renderer, 150, 150, 150, 255);
        }

        // Draw filled circle
        for (int dy = -static_cast<int>(DOT_RADIUS); dy <= static_cast<int>(DOT_RADIUS); ++dy)
        {
            for (int dx = -static_cast<int>(DOT_RADIUS); dx <= static_cast<int>(DOT_RADIUS); ++dx)
            {
                if (dx * dx + dy * dy <= static_cast<int>(DOT_RADIUS) * static_cast<int>(DOT_RADIUS))
                {
                    SDL_RenderPoint(m_renderer, static_cast<float>(dotX + dx), static_cast<float>(dotY + dy));
                }
            }
        }

        // Draw outline for selected
        if (isSelected)
        {
            SDL_SetRenderDrawColor(m_renderer, 0, 255, 0, 255);
            int outlineRadius = static_cast<int>(DOT_RADIUS + 2);
            for (int dy = -outlineRadius; dy <= outlineRadius; ++dy)
            {
                for (int dx = -outlineRadius; dx <= outlineRadius; ++dx)
                {
                    if (dx * dx + dy * dy >= outlineRadius * outlineRadius - 4 &&
                        dx * dx + dy * dy <= outlineRadius * outlineRadius + 4)
                    {
                        SDL_RenderPoint(m_renderer, static_cast<float>(dotX + dx), static_cast<float>(dotY + dy));
                    }
                }
            }
        }

        // Render components on this object
        RenderComponentsForObject(obj.get());
    }
}

dae::GameObject_Barebones* SceneViewport::GetGameObjectAtScreenPos(
    const std::vector<std::unique_ptr<dae::GameObject_Barebones>>& objects,
    int screenX, int screenY) const
{
    glm::vec2 screenPos(static_cast<float>(screenX), static_cast<float>(screenY));

    for (const auto& obj : objects)
    {
        glm::vec2 worldPos = obj->GetWorldPosition();
        glm::vec2 objScreenPos = WorldToScreen(worldPos);
        
        float dx = objScreenPos.x - screenX;
        float dy = objScreenPos.y - screenY;
        float distSq = dx * dx + dy * dy;
        
        if (distSq <= DOT_RADIUS * DOT_RADIUS)
        {
            return obj.get();
        }
    }
    
    return nullptr;
}

void SceneViewport::RenderComponentsForObject(dae::GameObject_Barebones* obj)
{
    if (!obj)
        return;

    const auto& components = obj->GetComponents();

    for (const auto& compHandle : components)
    {
        if (!compHandle)
            continue;

        // Use component type directly from the component instance
        std::string componentType = compHandle->componentType;

        if (componentType.empty())
            continue;

        // Render based on component type
        if (componentType == "TextRenderComponent")
        {
            // Render text with color
            auto textIt = compHandle->properties.find("text");
            auto colorIt = compHandle->properties.find("color");
            auto sizeIt = compHandle->properties.find("fontSize");

            if (textIt != compHandle->properties.end() && !textIt->second.empty() && m_font)
            {
                // Parse color
                SDL_Color textColor{ 255, 255, 255, 255 };
                if (colorIt != compHandle->properties.end() && !colorIt->second.empty())
                {
                    int r = 255, g = 255, b = 255, a = 255;
                    sscanf_s(colorIt->second.c_str(), "%d,%d,%d,%d", &r, &g, &b, &a);
                    textColor = { static_cast<Uint8>(r), static_cast<Uint8>(g), static_cast<Uint8>(b), static_cast<Uint8>(a) };
                }

                // Parse font size
                uint8_t fontSize = 16;
                if (sizeIt != compHandle->properties.end() && !sizeIt->second.empty())
                {
                    fontSize = static_cast<uint8_t>(std::stoi(sizeIt->second));
                }

                // Load font with appropriate size
                TTF_Font* renderFont = m_font;
                std::shared_ptr<dae::Font> sizedFont;
                if (fontSize != 16)
                {
                    try
                    {
                        sizedFont = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", fontSize);
                        renderFont = sizedFont->GetFont();
                    }
                    catch (const std::exception&)
                    {
                        renderFont = m_font;
                    }
                }

                // Create text texture
                using SurfacePtr = std::unique_ptr<SDL_Surface, decltype(&SDL_DestroySurface)>;
                using TempTexturePtr = std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)>;

                SurfacePtr textSurface(TTF_RenderText_Blended(renderFont, textIt->second.c_str(), 0, textColor), SDL_DestroySurface);
                if (textSurface)
                {
                    TempTexturePtr textTexture(SDL_CreateTextureFromSurface(m_renderer, textSurface.get()), SDL_DestroyTexture);
                    if (textTexture)
                    {
                        // Get world position and convert to screen
                        glm::vec2 worldPos = obj->GetWorldPosition();
                        glm::vec2 screenPos = WorldToScreen(worldPos);

                        int textX = static_cast<int>(screenPos.x);
                        int textY = static_cast<int>(screenPos.y);

                        SDL_FRect dstRect{ static_cast<float>(textX), static_cast<float>(textY),
                                         static_cast<float>(textSurface->w), static_cast<float>(textSurface->h) };
                        SDL_RenderTexture(m_renderer, textTexture.get(), nullptr, &dstRect);

                    }
                }
            }
        }
        else if (componentType == "TextureRenderComponent")
        {
            auto textureIt = compHandle->properties.find("textureName");
            if (textureIt != compHandle->properties.end() && !textureIt->second.empty())
            {
                glm::vec2 worldPos = obj->GetWorldPosition();
                glm::vec2 screenPos = WorldToScreen(worldPos);

                SDL_Texture* texture = GetOrLoadEditorTexture(textureIt->second);
                if (texture)
                {
                    SDL_FRect dstRect{};
                    dstRect.x = screenPos.x;
                    dstRect.y = screenPos.y;
                    SDL_GetTextureSize(texture, &dstRect.w, &dstRect.h);
                    SDL_RenderTexture(m_renderer, texture, nullptr, &dstRect);
                }
                else if (m_font)
                {
                    using SurfacePtr = std::unique_ptr<SDL_Surface, decltype(&SDL_DestroySurface)>;
                    using TempTexturePtr = std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)>;

                    std::string displayText = "[Tex: " + textureIt->second + "]";
                    SurfacePtr textSurface(TTF_RenderText_Blended(m_font, displayText.c_str(), 0, SDL_Color{ 200, 200, 200, 255 }), SDL_DestroySurface);
                    if (textSurface)
                    {
                        TempTexturePtr textTexture(SDL_CreateTextureFromSurface(m_renderer, textSurface.get()), SDL_DestroyTexture);
                        if (textTexture)
                        {
                            SDL_FRect dstRect{ screenPos.x, screenPos.y,
                                             static_cast<float>(textSurface->w), static_cast<float>(textSurface->h) };
                            SDL_RenderTexture(m_renderer, textTexture.get(), nullptr, &dstRect);
                        }
                    }
                }
            }
        }
    }
}

SDL_Texture* SceneViewport::GetOrLoadEditorTexture(const std::string& textureName)
{
    auto it = m_textureCache.find(textureName);
    if (it != m_textureCache.end())
    {
        return it->second.get();
    }

    const auto dataPath = dae::ResourceManager::GetInstance().GetDataPath();
    const auto fullPath = (dataPath / textureName).string();

    using SurfacePtr = std::unique_ptr<SDL_Surface, decltype(&SDL_DestroySurface)>;
    SurfacePtr surface(SDL_LoadPNG(fullPath.c_str()), SDL_DestroySurface);
    if (!surface)
    {
        return nullptr;
    }

    TexturePtr texture(SDL_CreateTextureFromSurface(m_renderer, surface.get()), SDL_DestroyTexture);

    if (!texture)
    {
        return nullptr;
    }

    auto [insertedIt, _] = m_textureCache.emplace(textureName, std::move(texture));
    return insertedIt->second.get();
}
