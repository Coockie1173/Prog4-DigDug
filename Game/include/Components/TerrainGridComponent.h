#ifndef _TERRAINGRIDCOMPONENT_H_
#define _TERRAINGRIDCOMPONENT_H_

#include "RenderComponent.h"
#include <Texture2D.h>
#include <glm/glm.hpp>
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <TerrainGridLevelLoader.h>

namespace dae
{
	enum class WallFlags : uint8_t
	{
		None = 0,
		Top = 1 << 0,
		Right = 1 << 1,
		Bottom = 1 << 2,
		Left = 1 << 3,
	};

	constexpr WallFlags operator|(WallFlags lhs, WallFlags rhs)
	{
		return static_cast<WallFlags>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
	}

	constexpr WallFlags operator&(WallFlags lhs, WallFlags rhs)
	{
		return static_cast<WallFlags>(static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs));
	}

	constexpr WallFlags operator~(WallFlags value)
	{
		return static_cast<WallFlags>(~static_cast<uint8_t>(value));
	}

	class TerrainGridComponent final : public RenderComponent
	{
	public:

		TerrainGridComponent(GameObject* Parent);

		void Render() const override;
		void Update() override;
		void LateUpdate() override;
		void Init() override;
		bool Deserialize(const std::map<std::string, std::string>& properties, std::string& errorMessage) override;

		void SetGridSize(int width, int height);
		void SetCellSize(float cellSize);
		void SetOrigin(const glm::vec2& origin);

		int GetWidth() const noexcept { return m_Width; }
		int GetHeight() const noexcept { return m_Height; }
		float GetCellSize() const noexcept { return m_CellSize; }

		glm::ivec2 WorldToCell(const glm::vec2& worldPosition) const;
		glm::vec2 CellToWorldCenter(const glm::ivec2& cell) const;

		bool IsValidCell(const glm::ivec2& cell) const;
		bool IsSolidCell(const glm::ivec2& cell) const;
		bool HasSolidAtWorldPosition(const glm::vec2& worldPosition) const;

		bool CarveAtWorldPosition(const glm::vec2& worldPosition, float radius = 12.0f);
		bool CarveCell(const glm::ivec2& cell);
		void Reset();

		uint8_t GetCellDepth(const glm::ivec2& cell) const;
		void SetCellDepth(const glm::ivec2& cell, uint8_t depth);
		uint8_t GetMaxDepth() const noexcept { return m_DefaultDepth; }
		bool CarveConnection(const glm::ivec2& from, const glm::ivec2& to);

		int IndexOf(const glm::ivec2& cell) const;
		uint8_t GetCellWalls(const glm::ivec2& cell) const
		{
			const auto index = IndexOf(cell);
			if (index < 0) return 0xFF;   // treat out-of-bounds as fully walled
			return m_CellWalls[static_cast<size_t>(index)];
		}

		void ApplyLevelData(const TerrainData& data);
		void SetLevelIndex(int index) { m_LevelIndex = index; }
		const TerrainData& GetLevelData() const { return m_LevelData; }

	private:
		int m_LevelIndex{ 0 };
		TerrainData   m_LevelData{};

		void ResizeStorage();
		void LoadTextures();
		void SpawnEnemy(const EnemySpawnData& ESD);

		// EXPOSE_TO_EDITOR("Width", "Number of cells horizontally")
		int m_Width{ 20 };
		// EXPOSE_TO_EDITOR("Height", "Number of cells vertically")
		int m_Height{ 15 };
		// EXPOSE_TO_EDITOR("Cell Size", "Size of a single terrain cell in world units")
		float m_CellSize{ 16.0f };
		uint8_t m_DefaultDepth{ 4 };
		std::vector<uint8_t> m_Cells{};
		std::vector<uint8_t> m_CellWalls;
		std::vector<std::shared_ptr<Texture2D>> m_DirtTextures{};
		std::shared_ptr<Texture2D> m_WallTexture;

		WallFlags GetWallBetween(const glm::ivec2& from, const glm::ivec2& to) const;
		WallFlags OppositeWall(WallFlags wall) const;
	};
}

#endif
