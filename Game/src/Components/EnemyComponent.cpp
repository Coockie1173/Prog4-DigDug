#include <ComponentFactoryRegistry.h>
#include <ComponentTypeMap.h>
#include <Components/EnemyComponent.h>
#include <ResourceManager.h>
#include <Components/SwappableRenderComponent.h>
#include <Components/EnemyStates/EnemyIdle.h>
#include <Components/EnemyStates/EnemyWander.h>
#include <Components/PlayerControllerComponent.h>
#include <Components/ObjectMoveComponent.h>

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
        m_pCurrentState->Update(*this);
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
}