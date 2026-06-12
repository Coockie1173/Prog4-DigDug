#include "SceneManager.h"
#include "Scene.h"

void dae::SceneManager::Update()
{
	for(auto& scene : m_scenes)
	{
		scene->FlushQueuedAdds();
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
	if (m_MustChangeScene)
	{
		ChangeScene();
		m_MustChangeScene = false;
		return;
	}

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

void dae::SceneManager::Clear()
{
	m_scenes.clear();
}

void dae::SceneManager::ChangeScene()
{
	this->Clear();

	auto& scene = this->CreateScene();
	Debugger::GetInstance().AttachToScene(&scene);

	std::string errorMessage;
	if (!SceneLoader::LoadSceneFromFile(m_EnqueuedScene, scene, errorMessage))
	{
		Debugger::GetInstance().LogError("Failed to load scene from '" + m_EnqueuedScene + "': " + errorMessage);
	}
	else
	{
		Debugger::GetInstance().LogDebug("Successfully loaded scene from '" + m_EnqueuedScene + "'");
		Debugger::GetInstance().LogDebug("Press F6 to remove these debug messages.");
	}
}
