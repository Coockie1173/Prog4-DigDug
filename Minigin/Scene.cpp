#include <algorithm>
#include "Scene.h"
#include <string>

using namespace dae;

void Scene::Add(std::unique_ptr<GameObject> object)
{
	assert(object != nullptr && "Cannot add a null GameObject to the scene.");
	if (!m_SceneHasInitialised)
	{
		object->SetScene(this);
		m_objects.emplace_back(std::move(object));
		return;
	}

	QueueAdd(std::move(object));
}

void Scene::QueueAdd(std::unique_ptr<GameObject> object)
{
	assert(object != nullptr && "Cannot queue a null GameObject to the scene.");
	m_pendingObjects.emplace_back(std::move(object));
}

void Scene::Remove(const GameObject& object)
{
	m_objects.erase(
		std::remove_if(
			m_objects.begin(),
			m_objects.end(),
			[&object](const auto& ptr) { return ptr.get() == &object; }
		),
		m_objects.end()
	);
}

void Scene::RemoveAll()
{
	m_objects.clear();
}

void dae::Scene::CleanupMarked()
{
	std::erase_if(m_objects, [](const std::unique_ptr<GameObject>& obj)
		{
			return obj->IsMarkedForDelete();
		});
}

//so we don't necessarily need access to m_objects outside of the scene
void dae::Scene::CleanupDebug()
{
	for (auto& object : m_objects)
	{
		if (object->IsDebug())
		{
			object->MarkForDeletion();
		}
	}
}

void dae::Scene::Init()
{
	for (auto& object : m_objects)
	{
		object->Init();
	}

	m_SceneHasInitialised = true;
	FlushQueuedAdds();
}

void dae::Scene::ForEachGameObject(const std::function<void(GameObject*)>& callback) const
{
	for (const auto& object : m_objects)
	{
		callback(object.get());
	}
}

void dae::Scene::FlushQueuedAdds()
{
	if (m_pendingObjects.empty())
	{
		return;
	}

	for (auto& object : m_pendingObjects)
	{
		object->SetScene(this);
		if (m_SceneHasInitialised)
		{
			object->Init();
		}
		m_objects.emplace_back(std::move(object));
	}
	m_pendingObjects.clear();
}

void Scene::Update()
{
	for(auto& object : m_objects)
	{
		object->Update();
	}
}

void dae::Scene::LateUpdate()
{
	for (auto& object : m_objects)
	{
		object->LateUpdate();
	}
}

void Scene::Render() const
{
	for (const auto& object : m_objects)
	{
		if (object->GetAttachedRenderer() == nullptr)
		{
			continue;
		}
		if (!object->IsDebug() || !object->GetAttachedRenderer()->GetPriority())
		{
			object->Render();
		}
	}

	// Ensure anything debug-related gets rendered last.
	for (const auto& object : m_objects)
	{
		if (object->GetAttachedRenderer() == nullptr)
		{
			continue;
		}
		if (object->IsDebug())
		{
			object->Render();
		}
	}

	for (const auto& object : m_objects)
	{
		if (object->GetAttachedRenderer() == nullptr)
		{
			continue;
		}
		if (object->GetAttachedRenderer()->GetPriority())
		{
			object->Render();
		}
	}
}

GameObject* dae::Scene::FindGameObject(const std::string ObjectName)
{
	for (const auto& GO : m_objects)
	{
		if (GO->GetName() == ObjectName)
		{
			return GO.get();
		}
	}
	return nullptr;
}

