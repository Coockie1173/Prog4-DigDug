#include <algorithm>
#include "Scene.h"
#include <string>

using namespace dae;

void Scene::Add(std::unique_ptr<GameObject> object)
{
	assert(object != nullptr && "Cannot add a null GameObject to the scene.");
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
		if (object->GetName().find("Debug") != std::string::npos)
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

