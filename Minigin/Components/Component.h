#ifndef _COMPONENT_H_
#define _COMPONENT_H_

namespace dae
{
	class GameObject;
	class Component
	{
	public:
		virtual ~Component() = default;

		Component(const Component&) = default;
		Component& operator=(const Component&) = default;
		Component(Component&&) noexcept = default;
		Component& operator=(Component&&) noexcept = default;

		virtual void Update() = 0;
		virtual void LateUpdate() = 0;
		virtual void Init() = 0;

	protected:
		GameObject* GetParent() const noexcept { return m_parent; };
		Component(dae::GameObject* Parent);

	private:
		GameObject* m_parent;
	};
}

#endif
