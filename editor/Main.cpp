#include <SDL3/SDL.h>
#include <imgui.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_sdlrenderer3.h>
#include "EditorScene.h"
#include "SceneViewport.h"
#include "EditorUI.h"

constexpr int WINDOW_WIDTH = 1024;
constexpr int WINDOW_HEIGHT = 576;

int main()
{
    if (!SDL_InitSubSystem(SDL_INIT_VIDEO))
    {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Minigin Editor",
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN
    );

    if (!window)
    {
        SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer)
    {
        SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
        return -1;
    }

    SDL_ShowWindow(window);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();

    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);

    EditorScene scene;
    SceneViewport viewport(renderer, WINDOW_WIDTH, WINDOW_HEIGHT);
    EditorUI ui;

    bool running = true;
    SDL_Event event;

    char newGameObjectName[256] = "GameObject";
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
        ui.RenderDialogs(scene, selectedObject, deleteTarget);

        ImGui::Render();

        SDL_SetRenderDrawColor(renderer, 45, 45, 48, 255);
        SDL_RenderClear(renderer);

        viewport.Render(scene.GetAllObjects(), selectedObject);

        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);

        SDL_RenderPresent(renderer);
    }

    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_QuitSubSystem(SDL_INIT_VIDEO);

    return 0;
}
