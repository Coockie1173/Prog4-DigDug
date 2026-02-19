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
#include "Debugger.h"

#include <filesystem>
#include "Components/TextureRendererComponent.h"
#include "Components/TextRendererComponent.h"
#include "Components/FPSCounterComponent.h"
#include "ResourceManager.h"
namespace fs = std::filesystem;

static void load()
{
	auto& scene = dae::SceneManager::GetInstance().CreateScene();
	Debugger::GetInstance().AttachToScene(&scene);
	//Debugger::GetInstance().LogError("some arbitrary error!");
	//Debugger::GetInstance().LogWarning("this shouldn't happen - I'll take care of it");
	//Debugger::GetInstance().LogDebug("you've reached line 26! nice!!");

	auto go = std::make_unique<dae::GameObject>("Background");
	std::string BGName{ "background.png" };
	go->AddComponent<dae::TextureRenderComponent>(BGName);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>("Logo");
	BGName = "logo.png";
	go->AddComponent<dae::TextureRenderComponent>(BGName);
	go->SetPosition(358, 180);
	scene.Add(std::move(go));
	
	auto font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 36);
	auto to = std::make_unique<dae::GameObject>("Text_Assignment");
	SDL_Color col{ 255,255,255,255 };
	to->AddComponent<dae::TextRenderComponent>("Programming 4 Assignment", col, font);
	to->SetPosition(292, 20);
	scene.Add(std::move(to));

	to = std::make_unique<dae::GameObject>("Text_FPS");
	to->AddComponent<dae::TextRenderComponent>("FPS: 0", col, font);
	to->AddComponent<dae::FPSCounterComponent>();
	to->SetPosition(10, 10);
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
