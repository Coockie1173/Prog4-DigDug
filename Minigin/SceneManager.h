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
					this->m_EnqueuedScene = std::any_cast<std::string>(SceneName);
					this->m_MustChangeScene = true;
				});
		};

		void ChangeScene();

		std::vector<std::unique_ptr<Scene>> m_scenes{};
		std::string m_EnqueuedScene{ "" };
		bool m_MustChangeScene{ false };

		EventManager::EventId m_LoadSceneEventID{ 0 };
	};
}
