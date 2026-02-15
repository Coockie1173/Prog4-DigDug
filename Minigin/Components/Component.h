#ifndef _COMPONENT_H_
#define _COMPONENT_H_

//reserved for special flags such as "am I a renderer?"
//most general components will just be marked generic

enum class ComponentType
{
	TYPE_RENDERER,
	TYPE_TRANSFORM,
	TYPE_GENERIC,
	TYPE_NONE, //this is reserved for component.cpp/h - no component should ever have this type
};

namespace dae
{
	class GameObject;
}

class Component
{
public:
	Component(dae::GameObject* Parent);
	virtual void Update() = 0;
	virtual void LateUpdate() = 0;
	virtual void Init() = 0;

	ComponentType GetType() { return m_myType; };

protected:
	ComponentType m_myType;
	//make sure we can also access whatever we're attached to
	dae::GameObject* m_parent;
};

#endif // !_COMPONENT_H_
