#include <Components/PumpComponent.h>

#include <ComponentFactoryRegistry.h>
#include <Hash.h>
#include <GameObject.h>
#include <Timing.h>

#include <algorithm>
#include <cmath>
#include <Components/EnemyComponent.h>
#include <Components/TerrainGridComponent.h>
#include <Scene.h>

#include <iostream>


dae::PumpComponent::PumpComponent(GameObject* Parent) : Component(Parent)
{}

void dae::PumpComponent::Configure(glm::vec2 direction, float travelDistance, float speed, std::function<void()> onFinished, std::function<void(GameObject*)> onHitEnemy)
{
	m_Direction = glm::length(direction) > 0.0f ? glm::normalize(direction) : glm::vec2{ 1.0f, 0.0f };
	m_TravelDistance = travelDistance;
	m_Speed = speed;
	m_TravelledDistance = 0.0f;
	m_OnFinished = std::move(onFinished);
	m_OnHitEnemy = std::move(onHitEnemy);
	m_IsConfigured = true;
	m_IsFinished = false;
	m_pCachedTerrainGrid = nullptr;
	m_CellInitialized = false;
}

void dae::PumpComponent::Update()
{
	if (!m_IsConfigured || m_IsFinished)
	{
		return;
	}

	auto* parent = GetParent();
	if (parent == nullptr)
	{
		FinishPump();
		return;
	}

	if (!m_CellInitialized)
	{
		if (auto* terrain = GetTerrain())
		{
			m_CurrentCell = terrain->WorldToCell(parent->GetWorldPosition());
			m_CellInitialized = true;
		}
	}

	if (m_CellInitialized)
	{
		if (auto* terrain = GetTerrain())
		{
			const glm::vec2 cellCenter = terrain->CellToWorldCenter(m_CurrentCell);
			const glm::vec2 toCenter = cellCenter - parent->GetWorldPosition();
			const float dot = glm::dot(toCenter, m_Direction);

			if (dot < 0.0f)
			{
				const uint8_t walls = terrain->GetCellWalls(m_CurrentCell);
				WallFlags movingInto = WallFlags::None;

				if (m_Direction.x > 0.5f) movingInto = WallFlags::Right;
				else if (m_Direction.x < -0.5f) movingInto = WallFlags::Left;
				else if (m_Direction.y > 0.5f) movingInto = WallFlags::Bottom;
				else if (m_Direction.y < -0.5f) movingInto = WallFlags::Top;

				if ((walls & static_cast<uint8_t>(movingInto)) != 0)
				{
					FinishPump();
					return;
				}

				m_CurrentCell += DirectionToCell();
			}
		}
	}

	const float dt = Timing::GetInstance().GetDeltaTime();
	auto currentPosition = parent->GetLocalPosition();
	currentPosition += m_Direction * m_Speed * dt;
	parent->SetLocalPosition(currentPosition);
	m_TravelledDistance += m_Speed * dt;

	if (m_OnHitEnemy)
	{
		auto* scene = parent->GetScene();
		if (scene != nullptr)
		{
			scene->ForEachGameObject([&](GameObject* obj)
				{
					if (m_IsFinished) return;
					if (obj == parent) return;
					if (obj->GetComponent<EnemyComponent>() == nullptr) return;
					if (obj->GetComponent<EnemyComponent>()->IsGhost()) return;

					if (parent->OverlapsWith(obj))
					{
						m_OnHitEnemy(obj);
						FinishPump();
					}
				});
		}
	}

	if (m_TravelledDistance >= m_TravelDistance)
	{
		FinishPump();
	}
}

void dae::PumpComponent::LateUpdate()
{}

void dae::PumpComponent::Init()
{}

bool dae::PumpComponent::Deserialize(const std::map<std::string, std::string>&, std::string&)
{
	return false;
}

void dae::PumpComponent::FinishPump()
{
	if (m_IsFinished)
		return;

	m_IsFinished = true;

	if (auto* parent = GetParent())
		parent->MarkForDeletion();

	if (m_OnFinished)
		m_OnFinished();
}

dae::TerrainGridComponent* dae::PumpComponent::GetTerrain()
{
	if (m_pCachedTerrainGrid)
		return m_pCachedTerrainGrid;

	auto* parent = GetParent();
	if (!parent) return nullptr;

	auto* scene = parent->GetScene();
	if (!scene) return nullptr;

	auto* terrainObject = scene->FindGameObject("Terrain");
	if (!terrainObject) return nullptr;

	m_pCachedTerrainGrid = terrainObject->GetComponent<dae::TerrainGridComponent>();
	return m_pCachedTerrainGrid;
}

glm::ivec2 dae::PumpComponent::DirectionToCell() const
{
	if (m_Direction.x > 0.5f) return { 1,  0 };
	else if (m_Direction.x < -0.5f) return { -1,  0 };
	else if (m_Direction.y > 0.5f) return { 0,  1 };
	else if (m_Direction.y < -0.5f) return { 0, -1 };
	return { 0, 0 };
}