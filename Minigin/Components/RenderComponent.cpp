#include "RenderComponent.h"
#include "../GameObject.h"
#include "Component.h"

RenderComponent::RenderComponent(GameObject* Parent) : Component(Parent)
{
	m_myType = ComponentType::TYPE_RENDERER;
	m_subtype = RenderSubtype::RENDER_NONE;
}
