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
		// EXPOSE_TO_EDITOR("Name", "Name of the game object")
		std::string m_objectName{};
		// EXPOSE_TO_EDITOR("Is Debug", "Whether this is a debug object")
		bool m_isDebugData{};
		bool m_markedForRemoval{};
		// EXPOSE_TO_EDITOR("Local Position", "Position relative to parent")
		glm::vec2 m_localPosition;
		// EXPOSE_TO_EDITOR("World Position", "Absolute position in world space")
		glm::vec2 m_worldPosition;

		bool m_positionIsDirty{false};

		void AddChild(GameObject* Child);
		void RemoveChild(GameObject* Child);
		void UpdateWorldPosition();
		void SetPositionDirty();

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

		template<typename T>
		inline T* GetComponent()
		{
			static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");

			for (auto& C : m_attachedComponents)
			{
				if (auto CastC = dynamic_cast<T*>(C.get()))
				{
					return CastC;
				}
			}

			return nullptr;
		}

		template<typename T>
		void GetComponentsOfType(std::vector<T*>& ReturnedComponents)
		{
			static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");

			for (auto& C : m_attachedComponents)
			{
				if (auto CastC = dynamic_cast<T*>(C))
				{
					ReturnedComponents.push_back(CastC);
				}
			}
		}

		template<typename T>
		void GetComponentIDsOfType(std::vector<int>& ReturnedComponentIDs)
		{
			int idx = 0;
			for (auto& C : m_attachedComponents)
			{
				if (auto CastC = dynamic_cast<T*>(C))
				{
					ReturnedComponentIDs.push_back(idx);
				}
				idx++;
			}
		}

		template<typename T>
		bool RemoveComponent()
		{
			static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");

			int idx = 0;
			for (auto& C : m_attachedComponents)
			{
				if (auto CastC = dynamic_cast<T*>(C))
				{
					m_attachedComponents.erase(m_attachedComponents.begin() + idx);
					return true;
				}
				idx++;
			}

			return false;
		}

		void RemoveComponentAtID(int ID)
		{
			m_attachedComponents.erase(m_attachedComponents.begin() + ID);
		}

		template<typename T>
		bool HasComponent()
		{
			static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");

			T* GC = GetComponent<T>();

			return GC != nullptr;
		}

		template <typename T, typename... Args>
		T* AddComponent(Args&&... args)
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
					return nullptr;
				}
				m_renderer = r;
			}

			m_attachedComponents.push_back(std::move(component));

			return componentPtr;
		}

	};
}
