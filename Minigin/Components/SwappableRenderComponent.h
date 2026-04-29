#ifndef _SWAPPABLE_RENDERER_H_
#define _SWAPPABLE_RENDERER_H_

#include "RenderComponent.h"

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
		void SetTexture(std::shared_ptr<Texture2D> newTexture) { m_pTexture = newTexture; }
	private:
		std::string m_textureName{};
	};
};

#endif