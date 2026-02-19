#include "Component.h"

namespace dae
{
	Component::Component(GameObject* Parent) : m_parent(Parent)
	{
		m_myType = ComponentType::TYPE_NONE;
	}
};
