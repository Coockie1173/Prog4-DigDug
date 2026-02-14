#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#if _DEBUG && __has_include(<vld.h>)
#include <vld.h>
#endif

#include "Minigin.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "TextObject.h"
#include "Scene.h"

#include <filesystem>
#include "Components/TextureRendererComponent.h"
#include "Components/TextRendererComponent.h"
namespace fs = std::filesystem;

static void load()
{
	auto& scene = dae::SceneManager::GetInstance().CreateScene();

	auto go = std::make_unique<dae::GameObject>();
	std::string BGName{ "background.png" };
	go->AddComponent<TextureRenderComponent>(BGName);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	BGName = "logo.png";
	go->AddComponent<TextureRenderComponent>(BGName);
	go->SetPosition(358, 180);
	scene.Add(std::move(go));
	
	auto font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 36);
	auto to = std::make_unique<dae::GameObject>();
	to->AddComponent<TextRenderComponent>("Programming 4 Assignment", (SDL_Color)(255,255,255,255), font);
	to->SetPosition(292, 20);
	scene.Add(std::move(to));
}

int main(int, char*[]) {
#if __EMSCRIPTEN__
	fs::path data_location = "";
#else
	fs::path data_location = "./Data/";
	if(!fs::exists(data_location))
		data_location = "../Data/";
#endif
	dae::Minigin engine(data_location);
	engine.Run(load);
    return 0;
}
