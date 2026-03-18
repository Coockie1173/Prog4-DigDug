#pragma once
#include "Singleton.h"
#include <functional>
#include <vector>
#include <map>
#include <any>
#include <memory>

namespace dae
{
	class EventManager final : public Singleton<EventManager>
	{
	public:
		using EventCallback = std::function<void(unsigned int, const std::any&)>;
		using EventId = size_t;

		EventId Subscribe(unsigned int eventHash, EventCallback callback);
		void Unsubscribe(unsigned int eventHash, EventId callbackId);
		void Publish(unsigned int eventHash, const std::any& data = std::any());
		void Clear();

		EventManager();
		~EventManager();

		EventManager(const EventManager&) = delete;
		EventManager(EventManager&&) = delete;
		EventManager& operator=(const EventManager&) = delete;
		EventManager& operator=(EventManager&&) = delete;

	private:
		struct Callback
		{
			EventId id;
			EventCallback func;
		};

		std::map<unsigned int, std::vector<Callback>> m_callbacks;
		EventId m_nextId{0};
	};
}
