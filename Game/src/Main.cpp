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
#include "ComponentRegistration.h"

#include <filesystem>
#include <string>

namespace fs = std::filesystem;

static void load()
{
	auto& scene = dae::SceneManager::GetInstance().CreateScene();
	Debugger::GetInstance().AttachToScene(&scene);
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

	if (argc > 1)
	{
		std::string sceneFilePath = argv[1];
		auto loadFromFile = [sceneFilePath]()
		{
			auto& scene = dae::SceneManager::GetInstance().CreateScene();
			Debugger::GetInstance().AttachToScene(&scene);

			std::string errorMessage;
			if (!dae::SceneLoader::LoadSceneFromFile(sceneFilePath, scene, errorMessage))
			{
				Debugger::GetInstance().LogError("Failed to load scene from '" + sceneFilePath + "': " + errorMessage);
			}
			else
			{
				Debugger::GetInstance().LogDebug("Successfully loaded scene from '" + sceneFilePath + "'");
			}
		};

		engine.Run(loadFromFile);
	}
	else
	{
		engine.Run(load);
	}

    return 0;
}