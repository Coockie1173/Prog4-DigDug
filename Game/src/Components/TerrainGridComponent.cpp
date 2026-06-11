#include <Components/TerrainGridComponent.h>
#include <ComponentFactoryRegistry.h>
#include <Hash.h>
#include <GameObject.h>
#include <Renderer.h>
#include <ResourceManager.h>
#include <algorithm>
#include <cmath>
#include <Components/EnemyComponent.h>
#include <Components/SwappableRenderComponent.h>
#include <Components/ObjectMoveComponent.h>
#include <memory>
#include <Scene.h>

namespace
{
	const bool TerrainGridComponentRegistered = dae::RegisterComponentFactoryFor<dae::TerrainGridComponent>(make_sdbm_hash("TerrainGridComponent"));
}

namespace dae
{
	TerrainGridComponent::TerrainGridComponent(GameObject* Parent)
		: RenderComponent(Parent)
	{
	}

	void TerrainGridComponent::Update()
	{
	}

	void TerrainGridComponent::LateUpdate()
	{
	}

	void TerrainGridComponent::Render() const
	{
		if (m_DirtTextures.empty()) return;

		const float wt = m_CellSize * 0.25f;
		const float half = m_CellSize * 0.5f;
		const float halfWt = wt * 0.5f;

		struct WallDef { glm::ivec2 dir; float dx, dy, w, h; float angle; SDL_FlipMode flip; };
		const WallDef defs[4] =
		{
			{ { 0, -1}, 0, 0, m_CellSize, wt, 0.f,  SDL_FLIP_VERTICAL },
			{ { 0, 1}, 0, m_CellSize - wt, m_CellSize, wt, 0.f, SDL_FLIP_NONE },
			{ {-1, 0}, -(half - halfWt), half - halfWt, m_CellSize, wt, 90.f, SDL_FLIP_NONE },
			{ { 1, 0}, m_CellSize - wt - (half - halfWt), half - halfWt, m_CellSize, wt, 90.f, SDL_FLIP_VERTICAL },
		};

		const uint8_t wallFlagBits[4] =
		{
			static_cast<uint8_t>(WallFlags::Top),
			static_cast<uint8_t>(WallFlags::Bottom),
			static_cast<uint8_t>(WallFlags::Left),
			static_cast<uint8_t>(WallFlags::Right),
		};

		auto& renderer = Renderer::GetInstance();

		for (int y = 0; y < m_Height; ++y)
		{
			for (int x = 0; x < m_Width; ++x)
			{
				const glm::ivec2 cell{ x, y };
				const auto depth = GetCellDepth(cell);
				const auto worldPos = CellToWorldCenter(cell) - glm::vec2(half, half);

				// Dirt pass
				if (depth != 0)
				{
					const auto textureIndex = std::min<std::size_t>(m_DirtTextures.size() - 1, static_cast<std::size_t>(depth - 1));
					const auto& texture = m_DirtTextures[textureIndex];
					if (!texture) continue;

					renderer.RenderTexture(*texture, worldPos.x, worldPos.y, m_CellSize, m_CellSize);
				}

				// Wall pass
				else if (m_WallTexture)
				{
					const auto idx = IndexOf(cell);
					if (idx < 0) continue;

					const uint8_t walls = m_CellWalls[static_cast<size_t>(idx)];
					if (walls == 0) continue;

					for (int i = 0; i < 4; ++i)
					{
						if (walls & wallFlagBits[i])
						{
							const auto& d = defs[i];
							renderer.RenderTexture(*m_WallTexture, worldPos.x + d.dx, worldPos.y + d.dy, d.w, d.h, d.angle, d.flip);
						}
					}
				}
			}
		}
	}

	void TerrainGridComponent::Init()
	{
		LoadTextures();
		if (m_Cells.empty() && m_Width > 0 && m_Height > 0)
		{
			ResizeStorage();
		}

		const std::string path = "Data/terrain/level" + std::to_string(m_LevelIndex) + ".tdbin";
		m_LevelData = TerrainLoader::Read(path);
		ApplyLevelData(m_LevelData);
	}

	void TerrainGridComponent::SpawnEnemy(const EnemySpawnData& ESD)
	{
		const std::string Directories[2]{ "pooka/", "fygar/" };

		auto NewEnemy = std::make_unique<GameObject>("Enemy");
		NewEnemy->SetLocalPosition(CellToWorldCenter(ESD.cell));

		EnemyComponent* EC = NewEnemy->AddComponent<EnemyComponent>();
		ITerrainDeserializeHelper* TDH = dynamic_cast<ITerrainDeserializeHelper*>(EC);
		TDH->PassData(static_cast<bool>(ESD.type), Directories[static_cast<uint8_t>(ESD.type)]);

		NewEnemy->AddComponent<SwappableRenderComponent>();
		NewEnemy->AddComponent<ObjectMoveComponent>();

		this->GetParent()->GetScene()->QueueAdd(std::move(NewEnemy));
	}

	void TerrainGridComponent::ApplyLevelData(const TerrainData& data)
	{
		for (const auto& cell : data.carvedCells)
		{
			CarveCell(cell);
		}

		//unused for now
		for (const auto& [from, to] : data.connections)
		{
			CarveConnection(from, to);
		}

		//now spawn all the enemies
		for (const auto& enem : data.enemySpawns)
		{
			SpawnEnemy(enem);
		}
	}


	bool TerrainGridComponent::Deserialize(const std::map<std::string, std::string>& properties, std::string& errorMessage)
	{
		std::string tmp;
		if (!GetRequiredProperty(properties, "Width", tmp, errorMessage, "TerrainGridComponent")) return false;
		m_Width = std::max(1, std::stoi(tmp));
		if (!GetRequiredProperty(properties, "Height", tmp, errorMessage, "TerrainGridComponent")) return false;
		m_Height = std::max(1, std::stoi(tmp));
		if (!GetRequiredProperty(properties, "CellSize", tmp, errorMessage, "TerrainGridComponent")) return false;
		m_CellSize = std::max(1.0f, std::stof(tmp));
		//m_DirtTextures.resize(4);

		ResizeStorage();
		LoadTextures();
		return true;
	}

	void TerrainGridComponent::SetGridSize(int width, int height)
	{
		m_Width = std::max(0, width);
		m_Height = std::max(0, height);
		ResizeStorage();
	}

	void TerrainGridComponent::SetCellSize(float cellSize)
	{
		m_CellSize = std::max(1.0f, cellSize);
	}

	glm::ivec2 TerrainGridComponent::WorldToCell(const glm::vec2& worldPosition) const
	{
		auto Parent = this->GetParent();		
		return {
			static_cast<int>(std::floor((worldPosition.x - Parent->GetWorldPosition().x) / m_CellSize)),
			static_cast<int>(std::floor((worldPosition.y - Parent->GetWorldPosition().y) / m_CellSize))
		};
	}

	glm::vec2 TerrainGridComponent::CellToWorldCenter(const glm::ivec2& cell) const
	{
		auto Parent = this->GetParent();
		return {
			Parent->GetWorldPosition().x + (static_cast<float>(cell.x) + 0.5f) * m_CellSize,
			Parent->GetWorldPosition().y + (static_cast<float>(cell.y) + 0.5f) * m_CellSize
		};
	}

	bool TerrainGridComponent::IsValidCell(const glm::ivec2& cell) const
	{
		return cell.x >= 0 && cell.y >= 0 && cell.x < m_Width && cell.y < m_Height;
	}

	bool TerrainGridComponent::IsSolidCell(const glm::ivec2& cell) const
	{
		const auto depth = GetCellDepth(cell);
		return depth > 0;
	}

	bool TerrainGridComponent::HasSolidAtWorldPosition(const glm::vec2& worldPosition) const
	{
		return IsSolidCell(WorldToCell(worldPosition));
	}


	bool TerrainGridComponent::CarveAtWorldPosition(const glm::vec2& worldPosition, float radius)
	{
		if (m_Cells.empty())
		{
			return false;
		}

		const auto minCell = WorldToCell(worldPosition - glm::vec2(radius));
		const auto maxCell = WorldToCell(worldPosition + glm::vec2(radius));
		bool changed = false;

		for (int y = minCell.y; y <= maxCell.y; ++y)
		{
			for (int x = minCell.x; x <= maxCell.x; ++x)
			{
				glm::ivec2 cell{ x, y };
				if (!IsValidCell(cell))
				{
					continue;
				}

				const auto cellCenter = CellToWorldCenter(cell);
				const auto delta = cellCenter - worldPosition;
				if (glm::dot(delta, delta) <= radius * radius)
				{
					changed |= CarveCell(cell);
				}
			}
		}

		return changed;
	}

	bool TerrainGridComponent::CarveCell(const glm::ivec2& cell)
	{
		if (!IsValidCell(cell))
		{
			return false;
		}

		const auto index = IndexOf(cell);
		if (index < 0)
		{
			return false;
		}

		auto& depth = m_Cells[static_cast<size_t>(index)];
		if (depth == 0)
		{
			return false;
		}

		depth = 0;

		m_CellWalls[static_cast<size_t>(index)] =
			static_cast<uint8_t>(
				WallFlags::Top |
				WallFlags::Bottom |
				WallFlags::Left |
				WallFlags::Right);

		const glm::ivec2 dirs[4] =
		{
			{ 0, -1 }, 
			{ 0,  1 }, 
			{ -1, 0 }, 
			{ 1,  0 }  
		};

		const WallFlags flags[4] =
		{
			WallFlags::Top,
			WallFlags::Bottom,
			WallFlags::Left,
			WallFlags::Right
		};

		const WallFlags opposite[4] =
		{
			WallFlags::Bottom,
			WallFlags::Top,
			WallFlags::Right,
			WallFlags::Left
		};

		for (int i = 0; i < 4; ++i)
		{
			const glm::ivec2 neighbour = cell + dirs[i];

			if (!IsValidCell(neighbour))
			{
				continue;
			}

			const auto nIdx = static_cast<size_t>(IndexOf(neighbour));

			if (m_Cells[nIdx] == 0)
			{
				m_CellWalls[static_cast<size_t>(index)] &= ~static_cast<uint8_t>(flags[i]);

				m_CellWalls[nIdx] &= ~static_cast<uint8_t>(opposite[i]);
			}
		}

		return true;
	}

	void TerrainGridComponent::Reset()
	{
		std::fill(m_Cells.begin(), m_Cells.end(), m_DefaultDepth);
	}

	uint8_t TerrainGridComponent::GetCellDepth(const glm::ivec2& cell) const
	{
		const auto index = IndexOf(cell);
		if (index < 0)
		{
			return 0;
		}

		return m_Cells[static_cast<size_t>(index)];
	}

	void TerrainGridComponent::SetCellDepth(const glm::ivec2& cell, uint8_t depth)
	{
		const auto index = IndexOf(cell);
		if (index < 0)
		{
			return;
		}

		m_Cells[static_cast<size_t>(index)] = depth;
	}

	int TerrainGridComponent::IndexOf(const glm::ivec2& cell) const
	{
		if (!IsValidCell(cell))
		{
			return -1;
		}

		return cell.y * m_Width + cell.x;
	}

	void TerrainGridComponent::ResizeStorage()
	{
		if (m_Width <= 0 || m_Height <= 0)
		{
			m_Cells.clear();
			return;
		}

		m_Cells.assign(static_cast<size_t>(m_Width * m_Height), 0);
		m_CellWalls.assign(static_cast<size_t>(m_Width * m_Height),	
			static_cast<uint8_t>(WallFlags::Top | WallFlags::Right | WallFlags::Bottom | WallFlags::Left));
		for (int y = 0; y < m_Height; ++y)
		{
			//we need a lil air to begin with, as a treat
			if (y <= 2)
			{
				for (int x = 0; x < m_Width; ++x)
				{
					m_CellWalls[static_cast<size_t>(y * m_Width + x)] = 0;
				}
				continue;
			}

			const auto depthBand = static_cast<uint8_t>(std::min(4, 1 + (((y - 1) * 4) / std::max(1, m_Height - 2))));
			for (int x = 0; x < m_Width; ++x)
			{
				m_Cells[static_cast<size_t>(y * m_Width + x)] = depthBand;
			}

		}
	}

	void TerrainGridComponent::LoadTextures()
	{
		if (!m_DirtTextures.empty())
		{
			return;
		}

		m_DirtTextures.emplace_back(ResourceManager::GetInstance().LoadTexture("dirt/Layer1.png"));
		m_DirtTextures.emplace_back(ResourceManager::GetInstance().LoadTexture("dirt/Layer2.png"));
		m_DirtTextures.emplace_back(ResourceManager::GetInstance().LoadTexture("dirt/Layer3.png"));
		m_DirtTextures.emplace_back(ResourceManager::GetInstance().LoadTexture("dirt/Layer4.png"));
		m_WallTexture = ResourceManager::GetInstance().LoadTexture("dirt/DirtEdge.png");
	}

	WallFlags TerrainGridComponent::GetWallBetween(const glm::ivec2& from, const glm::ivec2& to) const
	{
		const auto delta = to - from;

		if (delta == glm::ivec2{ 0,-1 })
		{
			return WallFlags::Top;
		}

		if (delta == glm::ivec2{ 1,0 })
		{
			return WallFlags::Right;
		}

		if (delta == glm::ivec2{ 0,1 })
		{
			return WallFlags::Bottom;
		}

		if (delta == glm::ivec2{ -1,0 })
		{
			return WallFlags::Left;
		}

		return WallFlags::None;
	}

	WallFlags TerrainGridComponent::OppositeWall(WallFlags wall) const
	{
		switch (wall)
		{
		case WallFlags::Top:
		{
			return WallFlags::Bottom;
		}

		case WallFlags::Bottom:
		{
			return WallFlags::Top;
		}

		case WallFlags::Left:
		{
			return WallFlags::Right;
		}

		case WallFlags::Right:
		{
			return WallFlags::Left;
		}

		default:
		{
			return WallFlags::None;
		}
		}
	}

	bool TerrainGridComponent::CarveConnection(const glm::ivec2& from, const glm::ivec2& to)
	{
		if (!IsValidCell(from))
		{
			return false;
		}

		if (!IsValidCell(to))
		{
			return false;
		}

		const auto wall = GetWallBetween(from, to);

		if (wall == WallFlags::None)
		{
			return false;
		}

		const auto opposite = OppositeWall(wall);

		auto& fromWalls = m_CellWalls[static_cast<size_t>(IndexOf(from))];

		auto& toWalls = m_CellWalls[static_cast<size_t>(IndexOf(to))];

		fromWalls &= ~static_cast<uint8_t>(wall);

		toWalls &= ~static_cast<uint8_t>(opposite);

		m_Cells[static_cast<size_t>(IndexOf(from))] = 0;
		m_Cells[static_cast<size_t>(IndexOf(to))] = 0;

		return true;
	}

	uint8_t TerrainGridComponent::GetOriginalDepthAt(const glm::ivec2& cell) const
	{
		if (!IsValidCell(cell)) return 0;

		if (cell.y <= 2) return 0;

		return static_cast<uint8_t>(std::min(4, 1 + (((cell.y - 1) * 4) / std::max(1, m_Height - 2))));
	}
}

