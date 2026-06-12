#pragma once
#include <Component.h>
#include <glm/glm.hpp>
#include <memory>
#include <Hash.h>

namespace dae
{
	class TerrainGridComponent;
	class Texture2D;

	class RockComponent final : public Component
	{
	public:
		RockComponent(GameObject* Parent);
		~RockComponent() override = default;

		void Update() override;
		void LateUpdate() override;
		void Init() override;
		bool Deserialize(const std::map<std::string, std::string>& properties, std::string& errorMessage) override;

		static constexpr unsigned int ROCKCRUSHSCOREHASH = make_sdbm_hash("RockCrushScore");

	private:
		enum class RockState { Idle, Falling, Breaking };

		void LoadTextures();
		TerrainGridComponent* GetTerrain();
		bool IsPlayerDirectlyBelow() const;
		void StartBreaking();

		TerrainGridComponent* m_pCachedTerrain{ nullptr };

		std::shared_ptr<Texture2D> m_IdleTexture{};
		std::shared_ptr<Texture2D> m_BreakTextures[3]{};

		RockState m_State{ RockState::Idle };
		float m_FallSpeed{ 80.f };
		float m_BreakFrameTimer{ 0.f };
		static constexpr float BREAK_FRAME_DURATION{ 0.1f };
		int m_BreakFrame{ 0 };
		bool m_TexturesLoaded{ false };
	};
}
