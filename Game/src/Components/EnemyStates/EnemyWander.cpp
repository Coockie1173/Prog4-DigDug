#include <Components/EnemyStates/EnemyWander.h>
#include <Components/SwappableRenderComponent.h>
#include <Components/EnemyComponent.h>
#include <Timing.h>
#include <Components/ObjectMoveComponent.h>
#include <Components/TerrainGridComponent.h>
#include <GameObject.h>
#include <SceneManager.h>

namespace dae
{
	void EnemyWanderState::Enter(IEnemyContext& ctx)
	{
		m_WaddleTimer = 0;
		ctx.GetRenderer()->SetTexture(ctx.GetTextureMap()[EnemyComponent::WalkFiles[m_WaddleFrame].hash]);

		OMC = ctx.GetOMC();

		auto* terrain = GetTerrain(ctx);
		const glm::vec2 worldPos = ctx.GetMe()->GetParent()->GetWorldPosition();

		m_CurrentCell = terrain ? terrain->WorldToCell(worldPos) : glm::ivec2{ 0, 0 };

		m_TargetCell = m_CurrentCell;
	}
	
	EnemyState* EnemyWanderState::Update(IEnemyContext& ctx)
	{
		const float dt = Timing::GetInstance().GetDeltaTime();

       m_WaddleTimer += dt;
        if (m_WaddleTimer > TIMEBETWEENFRAMES)
        {
            m_WaddleTimer = 0;
            m_WaddleFrame ^= 1;
            ctx.GetRenderer()->SetTexture(ctx.GetTextureMap()[EnemyComponent::WalkFiles[m_WaddleFrame].hash]);
        }

		auto* terrain = GetTerrain(ctx);
		if (!terrain) return nullptr;

		const glm::vec2 worldPos = ctx.GetMe()->GetParent()->GetWorldPosition();
		const glm::vec2 targetWorld = terrain->CellToWorldCenter(m_TargetCell);
		const glm::vec2 toTarget = targetWorld - worldPos;
		const float distSq = glm::dot(toTarget, toTarget);
		const float stepDist = WADDLESPEED * dt;

		if (distSq <= stepDist * stepDist)
		{
			//snap to grid
			ctx.GetMe()->GetParent()->SetLocalPosition(targetWorld);

			const glm::ivec2 arrivedCell = m_TargetCell;
			const glm::ivec2 cameFrom = m_CurrentCell;
			m_CurrentCell = arrivedCell;

			m_TargetCell = PickNextCell(arrivedCell, cameFrom, terrain);

			const glm::vec2 newTarget = terrain->CellToWorldCenter(m_TargetCell);
			const glm::vec2 delta = newTarget - targetWorld;
			if (glm::dot(delta, delta) > 0.f)
			{
				m_Direction = glm::normalize(delta);
			}

			OMC->MoveObject(m_Direction, WADDLESPEED);

			return nullptr;
		}

		return nullptr;
	}

	void EnemyWanderState::Exit(IEnemyContext&)
	{

	}

	dae::TerrainGridComponent* dae::EnemyWanderState::GetTerrain(IEnemyContext& ctx) const
	{
		if (m_pCachedTerrainGrid != nullptr)
			return m_pCachedTerrainGrid;

		auto* enemy = ctx.GetMe()->GetParent();
		if (enemy == nullptr)
			return nullptr;

		auto* scene = enemy->GetScene();
		if (scene == nullptr)
			return nullptr;

		auto* terrainObject = scene->FindGameObject("Terrain");
		if (terrainObject == nullptr)
			return nullptr;

		m_pCachedTerrainGrid = terrainObject->GetComponent<dae::TerrainGridComponent>();
		return m_pCachedTerrainGrid;
	}

	std::vector<glm::ivec2> EnemyWanderState::GetOpenNeighbours(const glm::ivec2& cell, TerrainGridComponent* terrain) const
	{
		//the four cardinal neighbours and the WallFlag that separates us from them.
		static const std::pair<glm::ivec2, WallFlags> kNeighbours[4] =
		{
			{ { 0, -1}, WallFlags::Top    },
			{ { 0,  1}, WallFlags::Bottom },
			{ {-1,  0}, WallFlags::Left   },
			{ { 1,  0}, WallFlags::Right  },
		};

		const int idx = terrain->IndexOf(cell);
		if (idx < 0) return {};

		const uint8_t walls = terrain->GetCellWalls(cell);   //see note below
		std::vector<glm::ivec2> open;

		for (auto& [delta, flag] : kNeighbours)
		{
			// Wall bit clear -> connection is open -> passable
			if ((walls & static_cast<uint8_t>(flag)) == 0)
			{
				const glm::ivec2 neighbour = cell + delta;
				if (terrain->IsValidCell(neighbour))
				{
					open.push_back(neighbour);
				}
			}
		}

		return open;
	}

	glm::ivec2 EnemyWanderState::PickNextCell(const glm::ivec2& current, const glm::ivec2& cameFrom, TerrainGridComponent* terrain) const
	{
		auto neighbours = GetOpenNeighbours(current, terrain);
		if (neighbours.empty())
		{
			return current;
		}

		//separate "forward / turns" from "reverse"
		std::vector<glm::ivec2> preferred, fallback;
		for (auto& n : neighbours)
		{
			if (n == cameFrom) 
			{
				fallback.push_back(n);
			}
			else               
			{
				preferred.push_back(n);
			}
		}

		auto& pool = preferred.empty() ? fallback : preferred;

		//bias toward continuing in the current direction if it is available
		const glm::ivec2 forward = current + (current - cameFrom);
		for (auto& n : pool)
		{
			if (n == forward)
			{
				return n;
			}
		}

		//otherwise pick randomly among the non-reverse options
		return pool[static_cast<size_t>(rand()) % pool.size()];
	}
};