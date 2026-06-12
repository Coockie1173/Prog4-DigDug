#include <Components/RockComponent.h>
#include <ComponentFactoryRegistry.h>
#include <Hash.h>
#include <GameObject.h>
#include <Timing.h>
#include <ResourceManager.h>
#include <EventManager.h>
#include <Scene.h>
#include <Components/TerrainGridComponent.h>
#include <Components/EnemyComponent.h>
#include <Components/PlayerControllerComponent.h>
#include <Components/SwappableRenderComponent.h>
#include <GameManager.h>

namespace
{
	constexpr uint32_t CRUSH_SCORE_TABLE[] = { 1000, 2500, 4000, 6000, 8000, 10000, 12000, 15000 };
	constexpr int CRUSH_TABLE_SIZE = static_cast<int>(sizeof(CRUSH_SCORE_TABLE) / sizeof(CRUSH_SCORE_TABLE[0]));
}

namespace dae
{
	RockComponent::RockComponent(GameObject* Parent) : Component(Parent) {}

	void RockComponent::Init()
	{
		LoadTextures();
		if (auto* rc = GetParent()->GetComponent<SwappableRenderComponent>())
		{
			rc->SetTexture(m_IdleTexture);
		}
	}

	void RockComponent::Update()
	{
		auto* terrain = GetTerrain();
		if (!terrain) return;

		auto* parent = GetParent();

		if (m_State == RockState::Breaking)
		{
			m_BreakFrameTimer += Timing::GetInstance().GetDeltaTime();
			if (m_BreakFrameTimer >= BREAK_FRAME_DURATION)
			{
				m_BreakFrameTimer = 0.f;
				if (auto* rc = parent->GetComponent<SwappableRenderComponent>())
				{
					rc->SetTexture(m_BreakTextures[m_BreakFrame]);
				}
				++m_BreakFrame;
				if (m_BreakFrame >= 3)
				{
					parent->MarkForDeletion();
				}
			}
			return;
		}

		const glm::vec2 worldPos = parent->GetWorldPosition();
		const glm::ivec2 cell = terrain->WorldToCell(worldPos);
		const glm::ivec2 below = cell + glm::ivec2{ 0, 1 };

		if (m_State == RockState::Idle)
		{
			if (!terrain->IsValidCell(below) || terrain->IsSolidCell(below))
			{
				return;
			}

			if (IsPlayerDirectlyBelow())
			{
				return;
			}

			m_State = RockState::Falling;
		}

		if (m_State == RockState::Falling)
		{
			if (IsPlayerDirectlyBelow())
			{
				m_State = RockState::Idle;
				{
					return;
				}
			}

			const float dt = Timing::GetInstance().GetDeltaTime();
			glm::vec2 pos = parent->GetLocalPosition();
			pos.y += m_FallSpeed * dt;
			parent->SetLocalPosition(pos);

			const glm::ivec2 newCell = terrain->WorldToCell(parent->GetWorldPosition());
			const glm::ivec2 newBelow = newCell + glm::ivec2{ 0, 1 };
			bool hitFloor = !terrain->IsValidCell(newBelow) || terrain->IsSolidCell(newBelow);

			int crushCount = 0;
			auto* scene = parent->GetScene();
			if (scene)
			{
				scene->ForEachGameObject([&](GameObject* obj) {
					if (obj == parent) return;
					auto* enemy = obj->GetComponent<EnemyComponent>();
					if (!enemy || enemy->IsGhost()) return;
					if (!parent->OverlapsWith(obj)) return;
					enemy->KillMe();
					++crushCount;
				});
			}

			if (crushCount > 0)
			{
				auto& gd = GameManager::GetInstance().GetGameData();
				const int idx = std::min(gd.m_RockCrushCount, CRUSH_TABLE_SIZE - 1);
				const uint32_t points = CRUSH_SCORE_TABLE[idx] * static_cast<uint32_t>(crushCount);
				gd.m_RockCrushCount += crushCount;
				EventManager::GetInstance().Publish(ScoreComponent::SCOREHASH, points);
				StartBreaking();
				return;
			}

			if (hitFloor)
			{
				StartBreaking();
			}
		}
	}

	void RockComponent::LateUpdate() {}

	bool RockComponent::Deserialize(const std::map<std::string, std::string>&, std::string&)
	{
		return true;
	}

	void RockComponent::LoadTextures()
	{
		if (m_TexturesLoaded) return;
		m_TexturesLoaded = true;
		m_IdleTexture = ResourceManager::GetInstance().LoadTexture("rock_idle.png");
		m_BreakTextures[0] = ResourceManager::GetInstance().LoadTexture("rock_break1.png");
		m_BreakTextures[1] = ResourceManager::GetInstance().LoadTexture("rock_break2.png");
		m_BreakTextures[2] = ResourceManager::GetInstance().LoadTexture("rock_break3.png");
	}

	TerrainGridComponent* RockComponent::GetTerrain()
	{
		if (m_pCachedTerrain) return m_pCachedTerrain;
		auto* parent = GetParent();
		if (!parent) return nullptr;
		auto* scene = parent->GetScene();
		if (!scene) return nullptr;
		auto* terrainObj = scene->FindGameObject("Terrain");
		if (!terrainObj) return nullptr;
		m_pCachedTerrain = terrainObj->GetComponent<TerrainGridComponent>();
		return m_pCachedTerrain;
	}

	bool RockComponent::IsPlayerDirectlyBelow() const
	{
		auto* parent = GetParent();
		auto* scene = parent->GetScene();
		if (!scene) return false;

		auto* terrain = const_cast<RockComponent*>(this)->GetTerrain();
		if (!terrain) return false;

		const glm::ivec2 rockCell = terrain->WorldToCell(parent->GetWorldPosition());

		bool found = false;
		scene->ForEachGameObject([&](GameObject* obj) {
			if (found) return;
			if (!obj->GetComponent<PlayerControllerComponent>()) return;
			const glm::ivec2 playerCell = terrain->WorldToCell(obj->GetWorldPosition());
			if (playerCell.x == rockCell.x && playerCell.y > rockCell.y)
				found = true;
		});
		return found;
	}

	void RockComponent::StartBreaking()
	{
		m_State = RockState::Breaking;
		m_BreakFrame = 0;
		m_BreakFrameTimer = 0.f;
		if (auto* rc = GetParent()->GetComponent<SwappableRenderComponent>())
			rc->SetTexture(m_BreakTextures[0]);
	}
}
