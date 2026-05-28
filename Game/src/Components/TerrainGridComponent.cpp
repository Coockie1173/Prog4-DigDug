#include <Components/TerrainGridComponent.h>
#include <ComponentFactoryRegistry.h>
#include <Hash.h>
#include <GameObject.h>
#include <Renderer.h>
#include <ResourceManager.h>
#include <algorithm>
#include <cmath>

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
		if (m_DirtTextures.empty())
		{
			return;
		}

		for (int y = 0; y < m_Height; ++y)
		{
			for (int x = 0; x < m_Width; ++x)
			{
				const glm::ivec2 cell{ x, y };
				const auto depth = GetCellDepth(cell);
				if (depth == 0)
				{
					continue;
				}

				const auto textureIndex = std::min<std::size_t>(m_DirtTextures.size() - 1, static_cast<std::size_t>(depth - 1));
				const auto& texture = m_DirtTextures[textureIndex];
				if (!texture)
				{
					continue;
				}

				const auto worldPos = CellToWorldCenter(cell) - glm::vec2(m_CellSize * 0.5f, m_CellSize * 0.5f);
				Renderer::GetInstance().RenderTexture(*texture, worldPos.x, worldPos.y, m_CellSize, m_CellSize);
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
	}

	bool TerrainGridComponent::Deserialize(const std::map<std::string, std::string>& properties, std::string&)
	{
		if (const auto it = properties.find("width"); it != properties.end())
		{
			m_Width = std::max(1, std::stoi(it->second));
		}
		if (const auto it = properties.find("height"); it != properties.end())
		{
			m_Height = std::max(1, std::stoi(it->second));
		}
		if (const auto it = properties.find("cellSize"); it != properties.end())
		{
			m_CellSize = std::max(1.0f, std::stof(it->second));
		}
		if (const auto it = properties.find("originX"); it != properties.end())
		{
			m_Origin.x = std::stof(it->second);
		}
		if (const auto it = properties.find("originY"); it != properties.end())
		{
			m_Origin.y = std::stof(it->second);
		}
		if (const auto it = properties.find("defaultDepth"); it != properties.end())
		{
			m_DefaultDepth = static_cast<uint8_t>(std::clamp(std::stoi(it->second), 0, 255));
		}

		if (const auto it = properties.find("layer1"); it != properties.end())
		{
			m_DirtTextures.resize(4);
		}

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

	void TerrainGridComponent::SetOrigin(const glm::vec2& origin)
	{
		m_Origin = origin;
	}

	glm::ivec2 TerrainGridComponent::WorldToCell(const glm::vec2& worldPosition) const
	{
		return {
			static_cast<int>(std::floor((worldPosition.x - m_Origin.x) / m_CellSize)),
			static_cast<int>(std::floor((worldPosition.y - m_Origin.y) / m_CellSize))
		};
	}

	glm::vec2 TerrainGridComponent::CellToWorldCenter(const glm::ivec2& cell) const
	{
		return {
			m_Origin.x + (static_cast<float>(cell.x) + 0.5f) * m_CellSize,
			m_Origin.y + (static_cast<float>(cell.y) + 0.5f) * m_CellSize
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
		for (int y = 0; y < m_Height; ++y)
		{
			const auto depthBand = static_cast<uint8_t>(std::min(4, 1 + ((y * 4) / std::max(1, m_Height - 1))));
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
	}
}
