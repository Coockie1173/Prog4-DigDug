#pragma once
#include <memory>
#include <string>
#include <vector>
#include "GameObject.h"

namespace dae
{
	class Scene final
	{
	public:
		void Add(std::unique_ptr<GameObject> object);
		void Remove(const GameObject& object);
		void RemoveAll();
		void CleanupMarked();
		void CleanupDebug();

		void Init();
		void Update();
		void LateUpdate();
		void Render() const;

		GameObject* FindGameObject(const std::string ObjectName);

		bool HasInitialised() { return m_SceneHasInitialised; };

		~Scene() = default;
		Scene(const Scene& other) = delete;
		Scene(Scene&& other) = delete;
		Scene& operator=(const Scene& other) = delete;
		Scene& operator=(Scene&& other) = delete;

	private:
		friend class SceneManager;
		explicit Scene() = default;

		std::vector < std::unique_ptr<GameObject>> m_objects{};
		bool m_SceneHasInitialised{false};
	};

}
