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

namespace dae
{
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
		glm::vec2 GetOrigin() const noexcept { return m_Origin; }

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

	private:
		int IndexOf(const glm::ivec2& cell) const;
		void ResizeStorage();
		void LoadTextures();

		// EXPOSE_TO_EDITOR("Width", "Number of cells horizontally")
		int m_Width{ 20 };
		// EXPOSE_TO_EDITOR("Height", "Number of cells vertically")
		int m_Height{ 15 };
		// EXPOSE_TO_EDITOR("Cell Size", "Size of a single terrain cell in world units")
		float m_CellSize{ 16.0f };
		// EXPOSE_TO_EDITOR("Origin", "World-space origin of the terrain grid")
		glm::vec2 m_Origin{ 0.0f, 0.0f };
		// EXPOSE_TO_EDITOR("Default Depth", "Initial dirt layers per cell")
		uint8_t m_DefaultDepth{ 4 };
		std::vector<uint8_t> m_Cells{};
		std::vector<std::shared_ptr<Texture2D>> m_DirtTextures{};
	};
}

#endif
