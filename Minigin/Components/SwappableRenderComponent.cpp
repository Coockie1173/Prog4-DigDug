#include "SwappableRenderComponent.h"
#include "../ComponentFactoryRegistry.h"
#include <ComponentTypeMap.h>
#include <Renderer.h>

namespace
{
	 const bool SwappableRenderComponentRegistered = dae::RegisterComponentFactoryFor<dae::SwappableRenderComponent>(dae::HASH_SwappableRenderComponent);
}

dae::SwappableRenderComponent::SwappableRenderComponent(GameObject* Parent) : RenderComponent(Parent)
{
}

void dae::SwappableRenderComponent::Render() const
{
	if (m_pTexture == nullptr)
	{
		return;
	}

	auto textureSize = m_pTexture->GetSize();
	auto pos = GetParent()->GetWorldPosition();
	Renderer::GetInstance().RenderTexture(*m_pTexture, pos.x, pos.y, textureSize.x, textureSize.y, m_Rotation, m_Flip);
}

void dae::SwappableRenderComponent::Update()
{
}

void dae::SwappableRenderComponent::LateUpdate()
{
}

void dae::SwappableRenderComponent::Init()
{
}

void dae::SwappableRenderComponent::SetFacingDirection(const glm::vec2& facingVector)
{
	if (facingVector.x > 0.0f)
	{
		SetRotationAndFlip(0.0f, SDL_FLIP_NONE);
	}
	else if (facingVector.x < 0.0f)
	{
		SetRotationAndFlip(0.0f, SDL_FLIP_HORIZONTAL);
	}
	else if (facingVector.y < 0.0f)
	{
		SetRotationAndFlip(-90.0f, SDL_FLIP_NONE);
	}
	else if (facingVector.y > 0.0f)
	{
		SetRotationAndFlip(90.0f, SDL_FLIP_NONE);
	}
}

bool dae::SwappableRenderComponent::Deserialize(const std::map<std::string, std::string>&, std::string&)
{
	//for now this renderer assumes the textures are added externally
	return true;
}
