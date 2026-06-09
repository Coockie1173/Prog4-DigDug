#ifndef _ENEMYCOMPONENT_H_
#define _ENEMYCOMPONENT_H_

#include <component.h>
#include <Texture2D.h>
#include <glm/glm.hpp>
#include <string>
#include <map>
#include <Components/EnemyStates/EnemyState.h>
#include <Hash.h>

namespace dae
{
	class SwappableRenderComponent;
	class EnemyComponent : public Component, public IEnemyContext
	{
	public:
		EnemyComponent(GameObject* Parent);
		void Update() override;
		void LateUpdate() override;
		void Init() override;
		bool Deserialize(const std::map<std::string, std::string>& properties, std::string& errorMessage) override;

		//interface nonsense
		std::unordered_map<unsigned int, std::shared_ptr<Texture2D>>& GetTextureMap() override
		{
			return TextureLinks;
		}

		SwappableRenderComponent* GetRenderer() override;

		struct NamedFile {
			const char* name;
			unsigned int hash;

			template<size_t N>
			consteval NamedFile(const char(&text)[N]) : name(text), hash(make_sdbm_hash(text)) {}
		};

		static_assert(NamedFile{ "Attack.png" }.hash == make_sdbm_hash("Attack.png"));

		//allows us to access both the hashes and actual filenames
		static constexpr NamedFile AttackFile{ "Attack.png" };
		static constexpr NamedFile SplatFile{ "Splat.png" };
		static constexpr NamedFile GhostFiles[2]{ "Ghost01.png", "Ghost02.png" };
		static constexpr NamedFile PumpFiles[4]{ "Pump01.png", "Pump02.png", "Pump03.png", "Pump04.png" };
		static constexpr NamedFile WalkFiles[2]{ "Walk01.png", "Walk02.png" };

	private:
		dae::SwappableRenderComponent* m_pRenderComponent{ nullptr };

		//we put all the textures in a directory (pooka/fygar) and then follow specific patterns to load them in
		//since behaviour is the same for both enemies (exept we have the funny fire spit I guess) we can do it like this
		// EXPOSE_TO_EDITOR("Directory", "Name of the directory that the sprites are in")
		std::string m_SpriteDirs{};
		//based on the items in ^ we'll precache/load in all textures we need
		//we can then just pass that data along to our states
		
		// EXPOSE_TO_EDITOR("CanAttack", "Can this enemy spit fire?")
		bool m_CanAttack{};

		std::unordered_map<unsigned int, std::shared_ptr<Texture2D>> TextureLinks;
		std::unique_ptr<StatePool<EnemyState>> m_pStatePool{ nullptr };
		EnemyState* m_pCurrentState{ nullptr };
	};
}

#endif