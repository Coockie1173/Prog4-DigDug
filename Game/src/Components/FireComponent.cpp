#include <Components/FireComponent.h>

#include <Components/SwappableRenderComponent.h>
#include <Components/PlayerControllerComponent.h>
#include <GameObject.h>
#include <Scene.h>
#include <Timing.h>

dae::FireComponent::FireComponent(GameObject* Parent) : Component(Parent) {}

void dae::FireComponent::Configure(glm::vec2 direction, std::shared_ptr<Texture2D> fire1, std::shared_ptr<Texture2D> fire2, std::shared_ptr<Texture2D> fire3)
{
    m_Direction = glm::length(direction) > 0.0f ? glm::normalize(direction) : glm::vec2{ 1.f, 0.f };

    m_Textures[0] = std::move(fire1);
    m_Textures[1] = std::move(fire2);
    m_Textures[2] = std::move(fire3);

    m_FrameTimer = 0.f;
    m_LifeTimer = 0.f;
    m_Frame = 0;
    m_IsConfigured = true;
}

void dae::FireComponent::Update()
{
    if (!m_IsConfigured)
    {
        return;
    }

    const float dt = Timing::GetInstance().GetDeltaTime();

    m_LifeTimer += dt;
    if (m_LifeTimer >= LIFETIME)
    {
        FinishFire();
        return;
    }

    m_FrameTimer += dt;
    if (m_FrameTimer >= FRAME_TIME)
    {
        m_FrameTimer = 0.f;
        m_Frame = (m_Frame + 1) % FRAME_COUNT;

        if (m_pRenderer && m_Textures[m_Frame])
        {
            m_pRenderer->SetTexture(m_Textures[m_Frame]);
        }
    }

    auto* parent = GetParent();
    if (!parent) return;

    auto* scene = parent->GetScene();
    if (!scene) return;

    scene->ForEachGameObject([&](GameObject* obj)
    {
        if (obj == parent) return;

        auto* player = obj->GetComponent<PlayerControllerComponent>();
        if (!player) return;

        if (parent->OverlapsWith(obj))
        {
            player->PlayerDeath();
            FinishFire();
        }
    });
}

void dae::FireComponent::LateUpdate() {}

void dae::FireComponent::Init()
{
    m_pRenderer = GetParent()->GetComponent<SwappableRenderComponent>();
    if (m_pRenderer && m_Textures[0])
    {
        m_pRenderer->SetTexture(m_Textures[0]);
    }
}

bool dae::FireComponent::Deserialize(const std::map<std::string, std::string>&, std::string&)
{
    return false;
}

void dae::FireComponent::FinishFire()
{
    m_IsConfigured = false;
    if (auto* parent = GetParent())
    {
        parent->MarkForDeletion();
    }
}
