#pragma once
#include "Scene.h"
#include "Singleton.h"
#include <EventManager.h>
#include <Hash.h>
#include <memory>
#include <string>
#include <string>
#include <vector>
#include <SceneLoader.h>

namespace dae
{
	class Scene;
	class SceneManager final : public Singleton<SceneManager>
	{
	public:
		Scene& CreateScene();
		void Clear();

		void Update();
		void LateUpdate();
		void Render();
		void Cleanup();
		void CheckScenesInited();

		static constexpr auto CHANGELEVELHASH = make_sdbm_hash("ChangeLevel");

	private:
		friend class Singleton<SceneManager>;
		SceneManager()
		{
			m_LoadSceneEventID = EventManager::GetInstance().Subscribe(SceneManager::CHANGELEVELHASH,
				[this](unsigned int, const std::any& SceneName)
				{
					std::string SN = std::any_cast<std::string>(SceneName);
					this->Clear();

					auto& scene = this->CreateScene();
					Debugger::GetInstance().AttachToScene(&scene);

					std::string errorMessage;
					if (!SceneLoader::LoadSceneFromFile(SN, scene, errorMessage))
					{
						Debugger::GetInstance().LogError("Failed to load scene from '" + SN + "': " + errorMessage);
					}
					else
					{
						Debugger::GetInstance().LogDebug("Successfully loaded scene from '" + SN + "'");
						Debugger::GetInstance().LogDebug("Press F6 to remove these debug messages.");
					}
				});
		};

		std::vector<std::unique_ptr<Scene>> m_scenes{};

		EventManager::EventId m_LoadSceneEventID{ 0 };
	};
}
