#include <Components/EnemyStates/EnemyFireState.h>
#include <Components/EnemyStates/EnemyWander.h>
#include <Components/EnemyComponent.h>
#include <Components/FireComponent.h>
#include <Components/SwappableRenderComponent.h>
#include <GameObject.h>
#include <ResourceManager.h>
#include <Scene.h>
#include <Timing.h>

namespace dae
{
    void EnemyFireState::Enter(IEnemyContext& ctx)
    {
        m_Timer = 0.f;
        m_FireSpawned = false;

        ctx.GetRenderer()->SetTexture(ctx.GetTextureMap()[EnemyComponent::AttackFile.hash]);
    }

    EnemyState* EnemyFireState::Update(IEnemyContext& ctx)
    {
        const float dt = Timing::GetInstance().GetDeltaTime();
        m_Timer += dt;

        if (!m_FireSpawned && m_Timer >= FIRE_WIND_UP)
        {
            m_FireSpawned = true;

            auto* enemy  = ctx.GetMe()->GetParent();
            auto* scene  = enemy ? enemy->GetScene() : nullptr;

            if (scene)
            {
                const glm::vec2 facing = ctx.GetFacing();  
                const glm::vec2 spawnPos = enemy->GetWorldPosition();

                auto& texMap = ctx.GetTextureMap();

                auto fireObj = std::make_unique<dae::GameObject>("EnemyFire", spawnPos);

                auto* renderer = fireObj->AddComponent<SwappableRenderComponent>();
                renderer->SetFacingDirection(facing);

                auto* fire = fireObj->AddComponent<FireComponent>();
                fire->Configure(facing, texMap[EnemyComponent::FireFiles[0].hash], texMap[EnemyComponent::FireFiles[1].hash],
                    texMap[EnemyComponent::FireFiles[2].hash]
                );

                scene->QueueAdd(std::move(fireObj));
            }
        }

        if (m_Timer >= FIRE_WIND_UP + 0.2f)
        {
            return m_pStatePool->Get<EnemyWanderState>();
        }

        return nullptr;
    }

    void EnemyFireState::Exit(IEnemyContext& ctx)
    {
        ctx.GetRenderer()->SetTexture(ctx.GetTextureMap()[EnemyComponent::WalkFiles[0].hash]);
    }
}
