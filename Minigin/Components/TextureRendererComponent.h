#ifndef _TEXTURERENDERERCOMPONENT_H_
#define _TEXTURERENDERERCOMPONENT_H_

#include <string>
#include <memory>
#include "RenderComponent.h"

class TextureRenderComponent final : RenderComponent
{
public:
	TextureRenderComponent(GameObject& Parent, const std::string& TextureName);
	void Render() const override;
private:
	std::shared_ptr<Texture2D> m_texture{};
};

#endif