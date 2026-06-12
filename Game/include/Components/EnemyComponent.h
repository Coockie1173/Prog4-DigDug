#ifndef _ENEMYCOMPONENT_H_
#define _ENEMYCOMPONENT_H_

#include <Component.h>
#include <Texture2D.h>
#include <glm/glm.hpp>
#include <string>
#include <map>
#include <Components/EnemyStates/EnemyState.h>
#include <Hash.h>
#include <EventManager.h>
#include <TerrainGridLevelLoader.h>
#include <cstdint>

namespace dae
{
    struct NamedFile
    {
        const char*  name;
        unsigned int hash;

        template<size_t N>
        consteval NamedFile(const char(&text)[N]) : name(text), hash(make_sdbm_hash(text))
        {}
    };

    class SwappableRenderComponent;
    class PlayerControllerComponent;
    class ObjectMoveComponent;

    class EnemyComponent : public Component, public IEnemyContext, public ITerrainDeserializeHelper
    {
    public:
        explicit EnemyComponent(GameObject* Parent);
        ~EnemyComponent();

        void Update() override;
        void LateUpdate() override;
        void Init() override;
        bool Deserialize(const std::map<std::string, std::string>& properties, std::string& errorMessage) override;

        std::unordered_map<unsigned int, std::shared_ptr<Texture2D>>& GetTextureMap() override
        {
            return TextureLinks;
        }

        SwappableRenderComponent* GetRenderer() override;
        ObjectMoveComponent* GetOMC() override;
        EnemyComponent* GetMe() override { return this; }
        void KillMe() override;

        glm::vec2 GetFacing() const override { return m_Facing; }
        void SetFacing(glm::vec2 f) override { m_Facing = f; }

        bool CanAttack() const override { return m_CanAttack; }

        void PassData(bool CanAttack, std::string SpriteDir) override
        {
            m_SpriteDirs = std::move(SpriteDir);
            m_CanAttack  = CanAttack;
        }

        static_assert(NamedFile{ "Attack.png" }.hash == make_sdbm_hash("Attack.png"));

        inline static constexpr NamedFile AttackFile { "Attack.png"  };
        inline static constexpr NamedFile SplatFile { "Splat.png"   };
        inline static constexpr NamedFile GhostFiles[2] { "Ghost01.png", "Ghost02.png" };
        inline static constexpr NamedFile PumpFiles[4] { "Pump01.png", "Pump02.png", "Pump03.png", "Pump04.png" };
        inline static constexpr NamedFile WalkFiles[2] { "Walk01.png", "Walk02.png" };

        inline static constexpr NamedFile FireFiles[3] { "Fire1.png", "Fire2.png", "Fire3.png"   };
        static constexpr int FIRE_FILE_COUNT = 3;

        inline static auto ENEMYSPAWNHASH{ make_sdbm_hash("Enemy has appeared!") };
        inline static auto ENEMYDEATHHASH{ make_sdbm_hash("Enemy has perished!") };

        void OnPumped(PlayerControllerComponent* Player);
        void OnAirBlownIntoEnemy();

        bool IsGhost()  const;
        bool IsPumped() const;

    private:
        dae::SwappableRenderComponent* m_pRenderComponent{ nullptr };

        // EXPOSE_TO_EDITOR("Directory", "Name of the directory that the sprites are in")
        std::string m_SpriteDirs{};

        // EXPOSE_TO_EDITOR("CanAttack", "Can this enemy spit fire?")
        bool m_CanAttack{ false };

        glm::vec2 m_Facing{ 1.f, 0.f };

        std::unordered_map<unsigned int, std::shared_ptr<Texture2D>> TextureLinks;
        std::unique_ptr<StatePool<EnemyState>> m_pStatePool{ nullptr };
        EnemyState* m_pCurrentState{ nullptr };

        const uint32_t KillValues[4]{ 200, 300, 400, 500 };

        void PlayerReady();

        EventManager::EventId m_PlayerReadyEventID{ 0 };
        ObjectMoveComponent*  OMC{ nullptr };
    };
}

#endif
