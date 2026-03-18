#include "EventManager.h"

dae::EventManager::EventManager() = default;

dae::EventManager::~EventManager()
{
	Clear();
}

dae::EventManager::EventId dae::EventManager::Subscribe(unsigned int eventHash, EventCallback callback)
{
	auto id = m_nextId++;
	m_callbacks[eventHash].push_back({id, callback});
	return id;
}

void dae::EventManager::Unsubscribe(unsigned int eventHash, EventId callbackId)
{
	auto it = m_callbacks.find(eventHash);
	if (it != m_callbacks.end())
	{
		auto& callbacks = it->second;
		callbacks.erase(
			std::remove_if(callbacks.begin(), callbacks.end(),
				[callbackId](const Callback& cb) { return cb.id == callbackId; }),
			callbacks.end()
		);
	}
}

void dae::EventManager::Publish(unsigned int eventHash, const std::any& data)
{
	auto it = m_callbacks.find(eventHash);
	if (it != m_callbacks.end())
	{
		for (const auto& callback : it->second)
		{
			callback.func(eventHash, data);
		}
	}
}

void dae::EventManager::Clear()
{
	m_callbacks.clear();
}
