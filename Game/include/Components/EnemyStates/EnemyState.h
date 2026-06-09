#ifndef _ENEMYSTATE_H_
#define _ENEMYSTATE_H_

#include <vector>
#include <memory>
#include <StateHelper.h>
#include <Texture2D.h>

namespace dae
{
	//for sending over data between the Enemy and the state
	//why not do it like the playercontext? I wanted to check if this was better or not!
	//conclusion? ask me again later
	struct IEnemyContext
	{
		virtual std::unordered_map<unsigned int, std::shared_ptr<Texture2D>>& GetTextureMap() = 0;
	};

	class EnemyComponent;
	class EnemyState
	{
	public:
		EnemyState(StatePool<EnemyState>* pStatePool) : m_pStatePool(pStatePool) {}

		virtual ~EnemyState() = default;

		EnemyState(const EnemyState&) = delete;
		EnemyState& operator=(const EnemyState&) = delete;
		EnemyState(EnemyState&&) noexcept = delete;
		EnemyState& operator=(EnemyState&&) noexcept = delete;

		virtual void Enter(IEnemyContext& EnemyCTX) = 0;
		virtual EnemyState* Update(IEnemyContext& EnemyCTX) = 0;
		virtual void Exit(IEnemyContext& EnemyCTX) = 0;
	protected:
		StatePool<EnemyState>* m_pStatePool{ nullptr };
	};
}

#endif