#ifndef _COMPONENT_H_
#define _COMPONENT_H_

#include <map>
#include <string>

namespace dae
{
	class GameObject;
	class SceneLoader;

	// NOEXPOSE
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

		bool GetIsDisabled() const { return ComponentDisabled; }
		void SetIsDisabled(bool isDisabled) { ComponentDisabled = isDisabled; }
		GameObject* GetParent() const noexcept { return m_parent; };

	protected:
		Component(dae::GameObject* Parent);
		bool ComponentDisabled{false};

		bool GetRequiredProperty(const std::map<std::string, std::string>& properties, const std::string& key, std::string& target, std::string& errorMessage, const std::string& componentName) const
		{
			auto it = properties.find(key);
			if (it == properties.end())
			{
				errorMessage = componentName + " requires '" + key + "' property.";
				return false;
			}

			target = it->second;
			return true;
		}

	private:
		friend class SceneLoader;

		// Deserialize is private and only SceneLoader can call it.
		// This enforces that component deserialization only happens during scene loading,
		// preventing accidental deserialization outside the controlled scene loading context.
		virtual bool Deserialize(const std::map<std::string, std::string>& properties, std::string& errorMessage) = 0;

		GameObject* m_parent;
	};
}

#endif
