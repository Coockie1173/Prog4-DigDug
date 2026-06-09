#ifndef _PLAYERSTATE_H_
#define _PLAYERSTATE_H_

#include <vector>
#include <memory>
#include <StateHelper.h>

namespace dae
{
	class PlayerControllerComponent;
	class SwappableRenderComponent;
	class PlayerState
	{
	public:
		PlayerState(StatePool<PlayerState>* pStatePool) : m_pStatePool(pStatePool) {}

		virtual ~PlayerState() = default;

		PlayerState(const PlayerState&) = delete;
		PlayerState& operator=(const PlayerState&) = delete;
		PlayerState(PlayerState&&) noexcept = delete;
		PlayerState& operator=(PlayerState&&) noexcept = delete;

		virtual void Enter(PlayerControllerComponent& Player) = 0;
		virtual PlayerState* Update(PlayerControllerComponent& Player) = 0;
		virtual void Exit(PlayerControllerComponent& Player) = 0;
	protected:
		StatePool<PlayerState>* m_pStatePool{ nullptr };
	};

};

#endif