#pragma once

#include <SDL3/SDL.h>
#include <vector>
#include <memory>
#include <unordered_map>
#include <string>
#include <glm/glm.hpp>
#include "../gencomponents/GameObject_Barebones.h"
#include "EditorComponentData.h"
#include <SDL3_ttf/SDL_ttf.h>

class EditorScene;

class SceneViewport
{
public:
    using TexturePtr = std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)>;

    SceneViewport(SDL_Renderer* renderer, int width, int height);
    ~SceneViewport();

    void SetSize(int width, int height);
    void Render(EditorScene& scene, const std::vector<std::unique_ptr<dae::GameObject_Barebones>>& objects,
                dae::GameObject_Barebones* selectedObject);

    dae::GameObject_Barebones* GetGameObjectAtScreenPos(const std::vector<std::unique_ptr<dae::GameObject_Barebones>>& objects,
                                                         int screenX, int screenY) const;

    void SetFont(TTF_Font* font) { m_font = font; }

private:
    SDL_Renderer* m_renderer;
    int m_width;
    int m_height;
    TTF_Font* m_font = nullptr;
    std::unordered_map<std::string, TexturePtr> m_textureCache;

    glm::vec2 ScreenToWorld(glm::vec2 screenPos) const;
    glm::vec2 WorldToScreen(glm::vec2 worldPos) const;
    void RenderComponentsForObject(EditorScene& scene, dae::GameObject_Barebones* obj);
    SDL_Texture* GetOrLoadEditorTexture(const std::string& textureName);

    const float DOT_RADIUS = 5.0f;
    const float SCALE = 0.5f;
};
