#ifndef _ENEMYWANDER_H_
#define _ENEMYWANDER_H_

#include "./EnemyState.h"
#include <glm/glm.hpp>

namespace dae
{
	class ObjectMoveComponent;
	class TerrainGridComponent;
	class EnemyWanderState final : public EnemyState
	{
	public:
		EnemyWanderState(StatePool<EnemyState>* pStatePool) : EnemyState(pStatePool) {};

		void Enter(IEnemyContext& ctx) override;
		EnemyState* Update(IEnemyContext& ctx) override;
		void Exit(IEnemyContext& ctx) override;

	private:
		static constexpr float PROBE_MARGIN = 2.f;
		glm::ivec2 m_CurrentCell{ -1, -1 };
		glm::ivec2 m_TargetCell{ -1, -1 };
		glm::vec2 m_Direction{ 1.f, 0.f };

		float m_WaddleTimer{ 0 };
		uint8_t m_WaddleFrame{ 0 };
		static constexpr float TIMEBETWEENFRAMES{ 0.25f };
		static constexpr float WADDLESPEED{ 40.f };
		ObjectMoveComponent* OMC{ nullptr };

		TerrainGridComponent* GetTerrain(IEnemyContext& ctx) const;
		std::vector<glm::ivec2> GetOpenNeighbours(const glm::ivec2& cell, TerrainGridComponent* terrain) const;
		glm::ivec2 PickNextCell(const glm::ivec2& current, const glm::ivec2& came_from, TerrainGridComponent* terrain) const;

		mutable TerrainGridComponent* m_pCachedTerrainGrid{ nullptr };

		float m_StuckTimer{ 0.f };
		glm::vec2 m_LastCheckedPos{};
		static constexpr float STUCK_CHECK_INTERVAL = 2.f;
		static constexpr float STUCK_DISTANCE_SQ = 4.f;   
	};
};

#endif