#pragma once
#include <string>
#include <memory>
#include <vector>
#include <glm/glm.hpp>

class Component;
class TransformComponent;
namespace dae
{
	class Texture2D;
	class GameObject final
	{
		//Transform m_transform{};
		//std::shared_ptr<Texture2D> m_texture{};
		std::shared_ptr<TransformComponent> m_transform{};
		std::vector<std::shared_ptr<Component>> m_attachedComponents{};
		void AddBaseComponents();
	public:
		void Init();
		void Update();
		void Render() const;

		void SetPosition(float x, float y);
		const glm::vec2& GetPosition();

		template<typename T, typename ...Args>
		std::shared_ptr<T> AddComponent(Args && ...args);

		GameObject();
		GameObject(float PosX, float PosY);
		GameObject(glm::vec2 Position);
		~GameObject();
		GameObject(const GameObject& other) = delete;
		GameObject(GameObject&& other) = delete;
		GameObject& operator=(const GameObject& other) = delete;
		GameObject& operator=(GameObject&& other) = delete;
	};
}
