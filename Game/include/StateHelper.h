#ifndef _STATEHELPER_H_
#define _STATEHELPER_H_

#include <vector>
#include <memory>

namespace dae
{

	template<typename T>
	class StatePool
	{
	public:
		StatePool() = default;
		~StatePool() = default;

		template<class S> S* Get()
		{
			static_assert(std::is_base_of<T, S>::value, "S must be a subclass of T");
			for (auto& state : m_StatePool)
			{
				if (auto castedState = dynamic_cast<S*>(state.get()))
				{
					return castedState;
				}
			}
			auto newState = std::make_unique<S>(this);
			auto newStatePtr = newState.get();
			m_StatePool.push_back(std::move(newState));
			return newStatePtr;
		}

	private:
		std::vector<std::unique_ptr<T>> m_StatePool;
	};
}

#endif