#ifndef _TEXTURERENDERERCOMPONENT_H_
#define _TEXTURERENDERERCOMPONENT_H_

#include <string>
#include <memory>
#include "RenderComponent.h"

namespace dae
{
	class Texture2D;

	class TextureRenderComponent final : public RenderComponent
	{
	public:
		TextureRenderComponent(dae::GameObject* Parent, const std::string& TextureName);
		void Render() const override;
		void Update() override;
		void LateUpdate() override;
		void Init() override;

		const std::string& GetTextureName() const { return m_textureName; }
	private:
		// EXPOSE_TO_EDITOR("Texture Name", "Name of the texture to render")
		std::string m_textureName{};
	};
};

#endif