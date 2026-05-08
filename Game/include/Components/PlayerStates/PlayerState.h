#ifndef _PLAYERSTATE_H_
#define _PLAYERSTATE_H_

#include <vector>
#include <memory>

namespace dae
{
	class PlayerControllerComponent;
	class SwappableRenderComponent;
	class PlayerStatePool;
	class PlayerState
	{
	public:
		PlayerState(PlayerStatePool* pStatePool) : m_pStatePool(pStatePool) {}

		virtual ~PlayerState() = default;

		PlayerState(const PlayerState&) = delete;
		PlayerState& operator=(const PlayerState&) = delete;
		PlayerState(PlayerState&&) noexcept = delete;
		PlayerState& operator=(PlayerState&&) noexcept = delete;

		virtual void Enter(PlayerControllerComponent& Player) = 0;
		virtual PlayerState* Update(PlayerControllerComponent& Player) = 0;
		virtual void Exit(PlayerControllerComponent& Player) = 0;
	protected:
		//since most if not all of the states will need to change the sprite, just store it
		PlayerStatePool* m_pStatePool{ nullptr };
	};

	class PlayerStatePool
	{
	public:
		PlayerStatePool() = default;
		~PlayerStatePool() = default;

		//only allocate a new state if it doesn't already exist, otherwise return the existing one
		template<class S> S* Get()
		{
			static_assert(std::is_base_of<PlayerState, S>::value, "S must be a subclass of PlayerState");
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
		std::vector<std::unique_ptr<PlayerState>> m_StatePool;
	};

};

#endif