#include "Component.h"

Component::Component(dae::GameObject* Parent) : m_parent(Parent)
{
	m_myType = ComponentType::TYPE_NONE;
}
