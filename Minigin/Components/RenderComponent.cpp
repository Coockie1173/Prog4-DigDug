#include "RenderComponent.h"
#include "../GameObject.h"
#include "Component.h"
#include "Renderer.h"

namespace dae
{
	RenderComponent::RenderComponent(dae::GameObject* Parent) : Component(Parent)
	{
	}
	void RenderComponent::RenderAssignedTexture() const
	{
		if (m_pTexture == nullptr)
		{
			return;
		}
		glm::vec2 pos = GetParent()->GetWorldPosition();
		Renderer::GetInstance().RenderTexture(*m_pTexture, pos.x, pos.y);
	}
};
