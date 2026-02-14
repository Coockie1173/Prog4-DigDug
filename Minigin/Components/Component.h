#ifndef _COMPONENT_H_
#define _COMPONENT_H_

#include "../GameObject.h"

//reserved for special flags such as "am I a renderer?"
//most general components will just be marked generic

using namespace dae;

enum class ComponentType
{
	TYPE_RENDERER,
	TYPE_TRANSFORM,
	TYPE_GENERIC,
	TYPE_NONE,
};

class Component
{
public:
	Component(GameObject& Parent);
	virtual void Update() = 0;
	virtual void LateUpdate() = 0;
	virtual void Init() = 0;

	ComponentType GetType() { return m_myType; };

protected:
	ComponentType m_myType;
	//make sure we can also access whatever we're attached to
	GameObject& m_parent;
};

#endif // !_COMPONENT_H_
