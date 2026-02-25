#pragma once
#include <string>
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include "Components/Component.h"
#include "Components/RenderComponent.h"
#include <iostream>
#include "Debugger.h"

class Component;
class RenderComponent;
namespace dae
{
	class GameObject final
	{
		//Transform m_transform{};
		//std::shared_ptr<Texture2D> m_texture{};
		RenderComponent* m_renderer{};
		std::vector<std::unique_ptr<Component>> m_attachedComponents{};
		std::vector<GameObject*> m_children{};
		GameObject* m_parent;
		std::string m_objectName{};
		bool m_isDebugData{};
		bool m_markedForRemoval{};
		glm::vec2 m_localPosition;
		glm::vec2 m_worldPosition;

		bool m_positionIsDirty{false};

		void AddChild(GameObject* Child);
		void RemoveChild(GameObject* Child);
		void UpdateWorldPosition();

		bool IsChildOf(GameObject* Parent);

	public:
		void Init();
		void Update();
		void LateUpdate();
		void Render() const;

		bool IsDebug() const noexcept { return m_isDebugData; };
		RenderComponent* GetAttachedRenderer() const noexcept { return m_renderer; };

		GameObject(std::string Name, GameObject* Parent = nullptr, bool IsDebugObject = false);
		GameObject(std::string Name, float PosX, float PosY, GameObject* Parent = nullptr, bool IsDebugObject = false);
		GameObject(std::string Name, glm::vec2 Position, GameObject* Parent = nullptr, bool IsDebugObject = false);
		~GameObject();
		GameObject(const GameObject& other) = delete;
		GameObject(GameObject&& other) = delete;
		GameObject& operator=(const GameObject& other) = delete;
		GameObject& operator=(GameObject&& other) = delete;
		const std::string& GetName() const noexcept { return m_objectName; };
		GameObject* GetParent() const noexcept { return m_parent; };
		void SetParent(GameObject* Parent, bool KeepWorldPos);

		void MarkForDeletion() noexcept;
		bool IsMarkedForDelete() const noexcept { return m_markedForRemoval; };

		void SetPosition(float x, float y);
		void SetPosition(const glm::vec2& position);
		glm::vec2& GetWorldPosition();
		glm::vec2 GetLocalPosition() const noexcept { return m_localPosition; };
		void SetLocalPosition(const glm::vec2& pos);

		/// <summary>
		/// Gets first of matching component
		/// </summary>
		/// <typeparam name="T">Component type to find</typeparam>
		/// <returns>The first matching component</returns>
		template <typename T>
		T* GetComponent();

		template <typename T>
		void GetComponentsOfType(std::vector<T*>& ReturnedComponents);

		template <typename T>
		void GetComponentIDsOfType(std::vector<int>& ReturnedComponentIDs);

		/// <summary>
		/// Removes first component of type
		/// </summary>
		/// <typeparam name="T">Component type to remove</typeparam>
		/// <returns>Success</returns>
		template <typename T>
		bool RemoveComponent();

		/// <summary>
		/// In case you have multiple of one type and need to remove a specific one.
		/// </summary>
		/// <typeparam name="T">Component Type</typeparam>
		/// <param name="ID">The ID of the component. This will "reshuffle" all other IDs</param>
		void RemoveComponentAtID(int ID);

		template <typename T>
		bool HasComponent();

		template <typename T, typename... Args>
		void AddComponent(Args&&... args)
		{
			static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");

			auto component = std::make_unique<T>(this, std::forward<Args>(args)...);
			T* componentPtr = component.get();

			if (auto r = dynamic_cast<RenderComponent*>(componentPtr))
			{
				if (m_renderer)
				{
					std::cout << "\033Warning: an object cannot have multiple renderers. Ignoring addition of renderer.\037\n";
					Debugger::GetInstance().LogWarning(m_objectName + " cannot have multiple renderers. Ignoring extra renderer.");
					return;
				}
				m_renderer = r;
			}

			m_attachedComponents.push_back(std::move(component));
		}

	};
}
