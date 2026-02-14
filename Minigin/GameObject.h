#pragma once
#include <string>
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include "Components/Component.h"
#include "Components/TransformComponent.h"
#include "Components/RenderComponent.h"

class Component;
class TransformComponent;
class RenderComponent;
namespace dae
{
	class Texture2D;
	class GameObject final
	{
		//Transform m_transform{};
		//std::shared_ptr<Texture2D> m_texture{};
		TransformComponent* m_transform{};
		RenderComponent* m_renderer{};
		std::vector<std::unique_ptr<Component>> m_attachedComponents{};
		void AddBaseComponents();

	public:
		void Init();
		void Update();
		void Render() const;

		void SetPosition(float x, float y);
		const glm::vec2& GetPosition();

		GameObject();
		GameObject(float PosX, float PosY);
		GameObject(glm::vec2 Position);
		~GameObject();
		GameObject(const GameObject& other) = delete;
		GameObject(GameObject&& other) = delete;
		GameObject& operator=(const GameObject& other) = delete;
		GameObject& operator=(GameObject&& other) = delete;

		template <typename T, typename... Args>
		void AddComponent(Args&&... args)
		{
			static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");

			auto component = std::make_unique<T>(this, std::forward<Args>(args)...);
			T* componentPtr = component.get(); // raw pointer to return

			// Handle base types separately
			if (component->GetType() == ComponentType::TYPE_TRANSFORM)
			{
				if (m_transform)
					return;
				m_transform = dynamic_cast<TransformComponent*>(componentPtr);
			}
			else if (component->GetType() == ComponentType::TYPE_RENDERER)
			{
				if (m_renderer)
					return;
				m_renderer = dynamic_cast<RenderComponent*>(componentPtr);
			}
			m_attachedComponents.push_back(std::move(component));
		}

	};
}
