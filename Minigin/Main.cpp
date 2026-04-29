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
#include <format>
#include "Components/TextureRendererComponent.h"
#include "Components/TextRendererComponent.h"
#include "Components/FPSCounterComponent.h"
#include "Components/SpinnerComponent.h"
#include "Components/ScoreComponent.h"
#include "ResourceManager.h"
#include "Components/ObjectMoveComponent.h"
#include "Commands/MovementInputCommand.h"
#include "InputManager.h"

#include <GamepadButton.h>

namespace fs = std::filesystem;

static void load()
{
	auto& scene = dae::SceneManager::GetInstance().CreateScene();
	Debugger::GetInstance().AttachToScene(&scene);
	// Default load function - can be overridden by passing scene file path as command-line argument
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

	// Check if a scene file path is provided as command-line argument
	if (argc > 1)
	{
		std::string sceneFilePath = argv[1];
		
		// Create a load function that loads from the specified scene file
		auto loadFromFile = [sceneFilePath]()
		{
			auto& scene = dae::SceneManager::GetInstance().CreateScene();
			Debugger::GetInstance().AttachToScene(&scene);
			
			std::string errorMessage;
			if (!dae::SceneLoader::LoadSceneFromFile(sceneFilePath, scene, errorMessage))
			{
				Debugger::GetInstance().LogError(std::format("Failed to load scene from '{}': {}", sceneFilePath, errorMessage));
			}
			else
			{
				Debugger::GetInstance().LogDebug(std::format("Successfully loaded scene from '{}'", sceneFilePath));
			}
		};
		
		engine.Run(loadFromFile);
	}
	else
	{
		// Use the default load function
		engine.Run(load);
	}

    return 0;
}
