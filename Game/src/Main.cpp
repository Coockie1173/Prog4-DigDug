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
#include "EventManager.h"
#include "SceneLoader.h"
#include "GameComponentRegistration.h"

#include <filesystem>
#include <string>
#include <ComponentRegistration.h>

namespace fs = std::filesystem;

static constexpr auto DEFAULT_SCENE_FILE = "TestScene.mbin";

static void loadSceneFromFile(const fs::path& sceneFilePath)
{
	auto& scene = dae::SceneManager::GetInstance().CreateScene();
	Debugger::GetInstance().AttachToScene(&scene);

	std::string errorMessage;
	if (!dae::SceneLoader::LoadSceneFromFile(sceneFilePath.string(), scene, errorMessage))
	{
		Debugger::GetInstance().LogError("Failed to load scene from '" + sceneFilePath.string() + "': " + errorMessage);
	}
	else
	{
		Debugger::GetInstance().LogDebug("Successfully loaded scene from '" + sceneFilePath.string() + "'");
		Debugger::GetInstance().LogDebug("Press F6 to remove these debug messages.");
	}
}

int main(int argc, char* argv[]) {
#if __EMSCRIPTEN__
	fs::path data_location = "";
#else
	fs::path data_location = "./Data/";
	if(!fs::exists(data_location))
		data_location = "../Data/";
#endif
	dae::Minigin engine(data_location);
	dae::RegisterBuiltinComponentFactories();
	dae::RegisterGameComponentFactories();

	auto defaultSceneFilePath = data_location / DEFAULT_SCENE_FILE;
	if (argc > 1)
	{
		defaultSceneFilePath = argv[1];
	}

	engine.Run([defaultSceneFilePath]()
	{
		loadSceneFromFile(defaultSceneFilePath);
	});

    return 0;
}