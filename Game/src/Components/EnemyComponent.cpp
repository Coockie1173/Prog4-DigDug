#include <ComponentFactoryRegistry.h>
#include <Components/EnemyComponent.h>
#include <Components/EnemyStates/EnemyIdle.h>
#include <Components/EnemyStates/EnemyPumped.h>
#include <Components/EnemyStates/EnemyWander.h>
#include <Components/EnemyStates/EnemyGhost.h>
#include <Components/ObjectMoveComponent.h>
#include <Components/PlayerControllerComponent.h>
#include <Components/ScoreComponent.h>
#include <Components/SwappableRenderComponent.h>
#include <Components/TerrainGridComponent.h>
#include <ComponentTypeMap.h>
#include <GameObject.h>
#include <ResourceManager.h>
#include <Scene.h>

namespace
{
    const bool EnemyComponentRegistered = dae::RegisterComponentFactoryFor<dae::EnemyComponent>(dae::HASH_EnemyComponent);
}

namespace dae
{
    EnemyComponent::EnemyComponent(GameObject* Parent) : Component(Parent)
    {}

    void EnemyComponent::Update()
    {
        if (auto* nextState = m_pCurrentState->Update(*this))
        {
            m_pCurrentState->Exit(*this);
            nextState->Enter(*this);
            m_pCurrentState = nextState;
        }
    }

    void EnemyComponent::LateUpdate()
    {}

    SwappableRenderComponent* EnemyComponent::GetRenderer()
    {
        return m_pRenderComponent;
    }

    ObjectMoveComponent* EnemyComponent::GetOMC()
    {
        if (OMC != nullptr)
        {
            return OMC;
        }

        OMC = this->GetParent()->GetComponent<ObjectMoveComponent>();

        if (OMC == nullptr)
        {
            Debugger::GetInstance().LogError("Warning: Gameobject " + this->GetParent()->GetName() + "\nhas no ObjectMoveComponent");
        }

        return OMC;
    }

    //you may be wondering why it's like this here and different for the player, the reason might shock you:
    //I did NOT think of it at the time, if I have enough time I'll refactor it like so, if I didn't have enough time
    //oh well, you'll have this comment to look at instead
    void EnemyComponent::Init()
    {
        static constexpr NamedFile AllFiles[]
        {
            AttackFile, SplatFile,
            GhostFiles[0], GhostFiles[1],
            PumpFiles[0], PumpFiles[1], PumpFiles[2], PumpFiles[3],
            WalkFiles[0], WalkFiles[1]
        };

        for (const auto& file : AllFiles)
        {
            TextureLinks.emplace(file.hash, ResourceManager::GetInstance().LoadTexture(m_SpriteDirs + file.name));
        }

        m_pRenderComponent = dynamic_cast<SwappableRenderComponent*>(GetParent()->GetAttachedRenderer());

        m_pStatePool = std::make_unique<StatePool<EnemyState>>();
        m_pCurrentState = m_pStatePool->Get<EnemyIdleState>();
        m_pCurrentState->Enter(*this);

        //once the player enters the center we can make the enemy waddle about
        m_PlayerReadyEventID = EventManager::GetInstance().Subscribe(PlayerControllerComponent::PLAYERREADYHASH,
            [this](unsigned int, const std::any&) { this->PlayerReady(); });
    }

    bool EnemyComponent::Deserialize(const std::map<std::string, std::string>& properties, std::string& errorMessage)
    {
        std::string CA{};
        if (!GetRequiredProperty(properties, "SpriteDirs", m_SpriteDirs, errorMessage, "PlayerControllerComponent")) return false;
        m_SpriteDirs += "/";
        if (!GetRequiredProperty(properties, "CanAttack", CA, errorMessage, "PlayerControllerComponent")) return false;
        m_CanAttack = CA == "true";
        return true;
    }

    void EnemyComponent::PlayerReady()
    {
        m_pCurrentState->Exit(*this);
        m_pCurrentState = m_pStatePool->Get<EnemyWanderState>();
        m_pCurrentState->Enter(*this);
    }

    void EnemyComponent::OnPumped(PlayerControllerComponent* pumper)
    {
        m_pCurrentState->Exit(*this);
        auto* pumped = m_pStatePool->Get<EnemyPumpedState>();
        pumped->SetPumper(pumper);
        pumped->Enter(*this);
        m_pCurrentState = pumped;
    }

    void EnemyComponent::OnAirBlownIntoEnemy()
    {
        if (m_pCurrentState == m_pStatePool->Get<EnemyPumpedState>())
        {
            auto p = dynamic_cast<EnemyPumpedState*>(m_pCurrentState);
            p->OnAirPumped();
        }
    }

    void EnemyComponent::KillMe()
    {
        uint8_t layerIndex = 0;

        auto* scene = this->GetParent()->GetScene();
        if (scene != nullptr)
        {
            auto* terrainObject = scene->FindGameObject("Terrain");
            if (terrainObject != nullptr)
            {
                auto* terrain = terrainObject->GetComponent<TerrainGridComponent>();
                if (terrain != nullptr)
                {
                    const glm::vec2 worldPos = this->GetParent()->GetWorldPosition();
                    const glm::ivec2 cell = terrain->WorldToCell(worldPos);

                    uint8_t depth = terrain->GetCellDepth(cell);
                    if (depth == 0)
                        depth = terrain->GetOriginalDepthAt(cell);

                    layerIndex = (depth > 0) ? (depth - 1) : 0;
                }
            }
        }

        uint32_t ScoreVal = KillValues[layerIndex] * (m_CanAttack ? 2 : 1);
        EventManager::GetInstance().Publish(ScoreComponent::SCOREHASH, ScoreVal);
        this->GetParent()->MarkForDeletion();
    }

    EnemyComponent::~EnemyComponent()
    {
        //always unsubscribe when we kill an object
        EventManager::GetInstance().Unsubscribe(PlayerControllerComponent::PLAYERREADYHASH, m_PlayerReadyEventID);
    }

    bool EnemyComponent::IsGhost() const
    {
        return m_pCurrentState == m_pStatePool->Get<EnemyGhostState>();
    }
}