#pragma once

#include <SDL3/SDL.h>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "../gencomponents/GameObject_Barebones.h"

class SceneViewport
{
public:
    SceneViewport(SDL_Renderer* renderer, int width, int height);
    
    void SetSize(int width, int height);
    void Render(const std::vector<std::unique_ptr<dae::GameObject_Barebones>>& objects,
                dae::GameObject_Barebones* selectedObject);
    
    dae::GameObject_Barebones* GetGameObjectAtScreenPos(const std::vector<std::unique_ptr<dae::GameObject_Barebones>>& objects,
                                                         int screenX, int screenY) const;

private:
    SDL_Renderer* m_renderer;
    int m_width;
    int m_height;
    
    glm::vec2 ScreenToWorld(glm::vec2 screenPos) const;
    glm::vec2 WorldToScreen(glm::vec2 worldPos) const;
    
    const float DOT_RADIUS = 5.0f;
    const float SCALE = 0.5f;
};
