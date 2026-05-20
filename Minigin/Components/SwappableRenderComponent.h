#ifndef _SWAPPABLE_RENDERER_H_
#define _SWAPPABLE_RENDERER_H_

#include "RenderComponent.h"
#include <SDL3/SDL.h>
#include <glm/glm.hpp>

namespace dae
{
	class SwappableRenderComponent final : public RenderComponent
	{
	public:
		SwappableRenderComponent(GameObject* Parent);
		void Render() const override;
		void Update() override;
		void LateUpdate() override;
		void Init() override;
		bool Deserialize(const std::map<std::string, std::string>& properties, std::string& errorMessage) override;
		void SetTexture(std::shared_ptr<dae::Texture2D> newTexture) { m_pTexture = newTexture; }
		std::shared_ptr<dae::Texture2D> GetTexture() const { return m_pTexture; }
		void SetRotationAndFlip(float rotation, SDL_FlipMode flip) { m_Rotation = rotation; m_Flip = flip; }
		void SetFacingDirection(const glm::vec2& facingVector);
	private:
		std::string m_textureName{};
		float m_Rotation{ 0.0f };
		SDL_FlipMode m_Flip{ SDL_FLIP_NONE };
	};
};

#endif