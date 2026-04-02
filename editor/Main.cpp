#include <SDL3/SDL.h>
#include <imgui.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_sdlrenderer3.h>
#include <filesystem>
#include <memory>
#include "EditorScene.h"
#include "SceneViewport.h"
#include "EditorUI.h"
#include "../Minigin/ResourceManager.h"
#include "../Minigin/Font.h"

constexpr int WINDOW_WIDTH = 1024;
constexpr int WINDOW_HEIGHT = 576;

int main()
{
    using WindowPtr = std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)>;
    using RendererPtr = std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)>;

    if (!SDL_InitSubSystem(SDL_INIT_VIDEO))
    {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return -1;
    }

    struct SDLShutdownGuard
    {
        ~SDLShutdownGuard() { SDL_QuitSubSystem(SDL_INIT_VIDEO); }
    } sdlGuard;

    WindowPtr window(SDL_CreateWindow(
        "Minigin Editor",
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN
    ), SDL_DestroyWindow);

    if (!window)
    {
        SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
        return -1;
    }

    RendererPtr renderer(SDL_CreateRenderer(window.get(), nullptr), SDL_DestroyRenderer);
    if (!renderer)
    {
        SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());
        return -1;
    }

    SDL_ShowWindow(window.get());

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();

    struct ImGuiShutdownGuard
    {
        ~ImGuiShutdownGuard()
        {
            ImGui_ImplSDLRenderer3_Shutdown();
            ImGui_ImplSDL3_Shutdown();
            ImGui::DestroyContext();
        }
    } imguiGuard;

    ImGui_ImplSDL3_InitForSDLRenderer(window.get(), renderer.get());
    ImGui_ImplSDLRenderer3_Init(renderer.get());

    std::filesystem::path dataPath = std::filesystem::path(__FILE__).parent_path() / "Data";
    if (!std::filesystem::exists(dataPath))
    {
        dataPath = std::filesystem::path(__FILE__).parent_path().parent_path() / "Data";
    }
    dae::ResourceManager::GetInstance().Init(dataPath);

    auto font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 16);
    TTF_Font* ttfFont = font->GetFont();

    EditorScene scene;
    SceneViewport viewport(renderer.get(), WINDOW_WIDTH, WINDOW_HEIGHT);
    viewport.SetFont(ttfFont);
    EditorUI ui;

    bool running = true;
    SDL_Event event;

    dae::GameObject_Barebones* selectedObject = nullptr;
    dae::GameObject_Barebones* deleteTarget = nullptr;

    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL3_ProcessEvent(&event);

            if (event.type == SDL_EVENT_QUIT)
            {
                running = false;
            }
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT)
            {
                int mouseX = static_cast<int>(event.button.x);
                int mouseY = static_cast<int>(event.button.y);

                if (!ImGui::GetIO().WantCaptureMouse)
                {
                    dae::GameObject_Barebones* clickedObject = viewport.GetGameObjectAtScreenPos(
                        scene.GetAllObjects(), mouseX, mouseY);
                    if (clickedObject)
                    {
                        selectedObject = clickedObject;
                    }
                }
            }
        }

        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        if (ImGui::IsKeyPressed(ImGuiKey_Escape))
        {
            running = false;
        }

        ui.RenderSceneGraphPanel(scene, selectedObject, deleteTarget);
        ui.RenderPropertiesPanel(selectedObject);
        ui.RenderComponentsPanel(scene, selectedObject);
        ui.RenderDialogs(scene, selectedObject, deleteTarget);

        ImGui::Render();

        SDL_SetRenderDrawColor(renderer.get(), 45, 45, 48, 255);
        SDL_RenderClear(renderer.get());

        viewport.Render(scene, scene.GetAllObjects(), selectedObject);

        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer.get());

        SDL_RenderPresent(renderer.get());
    }

    return 0;
}
