#ifndef _COMPONENT_H_
#define _COMPONENT_H_

enum class ComponentType
{
	TYPE_RENDERER,
	TYPE_TRANSFORM,
	TYPE_GENERIC,
	TYPE_NONE,
};

namespace dae
{
	class GameObject;
	class Component
	{
	public:
		Component(dae::GameObject* Parent);
		virtual ~Component() = default;

		Component(const Component&) = default;
		Component& operator=(const Component&) = default;
		Component(Component&&) noexcept = default;
		Component& operator=(Component&&) noexcept = default;

		virtual void Update() = 0;
		virtual void LateUpdate() = 0;
		virtual void Init() = 0;

		ComponentType GetType() const noexcept { return m_myType; };

	protected:
		ComponentType m_myType;
		GameObject* GetParent() const noexcept { return m_parent; };

	private:
		GameObject* m_parent;
	};
}

#endif
