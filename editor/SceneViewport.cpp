#include "SceneViewport.h"
#include <cmath>

SceneViewport::SceneViewport(SDL_Renderer* renderer, int width, int height)
    : m_renderer(renderer), m_width(width), m_height(height)
{
}

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
