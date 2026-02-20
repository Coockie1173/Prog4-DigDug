#include "SceneManager.h"
#include "Scene.h"

void dae::SceneManager::Update()
{
	for(auto& scene : m_scenes)
	{
		scene->Update();
	}
}

void dae::SceneManager::LateUpdate()
{
	for(auto& scene : m_scenes)
	{
		scene->LateUpdate();
	}
}

void dae::SceneManager::Render()
{
	for (const auto& scene : m_scenes)
	{
		scene->Render();
	}
}

void dae::SceneManager::Cleanup()
{
	for (auto& scene : m_scenes)
	{
		scene->CleanupMarked();
	}
}

void dae::SceneManager::CheckScenesInited()
{
	for (auto& scene : m_scenes)
	{
		if (!scene->HasInitialised())
		{
			scene->Init();
		}
	}
}

dae::Scene& dae::SceneManager::CreateScene()
{
	m_scenes.emplace_back(new Scene());
	return *m_scenes.back();
}
