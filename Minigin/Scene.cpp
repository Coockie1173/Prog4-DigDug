#include <algorithm>
#include "Scene.h"
#include <string>

using namespace dae;

void Scene::Add(std::unique_ptr<GameObject> object)
{
	assert(object != nullptr && "Cannot add a null GameObject to the scene.");
	if (m_SceneHasInitialised) //in case an object gets added during runtime, we can still init it after the scene has already initialised the rest
	{
		object->Init();
	}
	m_objects.emplace_back(std::move(object));
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
	//after everything has been updated, loop through all objects see who's marked for deletion
	//anyone marked gets taken out of the array (or rather: left out of the array)
	//this should be safer than just yoinking them out of the array
	std::vector<std::unique_ptr<GameObject>> filtered;
	filtered.reserve(m_objects.size());

	for (auto& obj : m_objects)
	{
		if (!obj->IsMarkedForDelete())
		{
			filtered.push_back(std::move(obj));
		}
	}

	m_objects = std::move(filtered);
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
}

void Scene::Update()
{
	for(auto& object : m_objects)
	{
		object->Update();
	}
}

void Scene::Render() const
{
	std::vector<int> DebugItems{};
	int CurrentObjectIndex{-1};
	for (const auto& object : m_objects)
	{
		CurrentObjectIndex++;
		if (object->IsDebug())
		{
			DebugItems.push_back(CurrentObjectIndex);
			continue;
		}
		object->Render();
	}

	//ensure anything debug related gets rendered last
	for (const auto idx : DebugItems)
	{
		m_objects[idx]->Render();
	}
}

