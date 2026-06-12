#ifndef _FIRECOMPONENT_H_
#define _FIRECOMPONENT_H_

#include <Component.h>
#include <glm/glm.hpp>
#include <memory>

namespace dae
{
    class SwappableRenderComponent;
    class Texture2D;

    // NOEXPOSE  (auto-added by EnemyFireState, not placed via editor)
    class FireComponent final : public Component
    {
    public:
        explicit FireComponent(GameObject* Parent);

        void Configure(glm::vec2 direction, std::shared_ptr<Texture2D> fire1, std::shared_ptr<Texture2D> fire2, std::shared_ptr<Texture2D> fire3);

        void Update() override;
        void LateUpdate() override;
        void Init() override;
        bool Deserialize(const std::map<std::string, std::string>& properties, std::string& errorMessage) override;

    private:
        static constexpr float FRAME_TIME = 0.12f; 
        static constexpr int FRAME_COUNT = 3;
        static constexpr float LIFETIME = 0.8f;  

        glm::vec2 m_Direction{ 1.f, 0.f };

        std::shared_ptr<Texture2D> m_Textures[FRAME_COUNT]{};

        SwappableRenderComponent* m_pRenderer{ nullptr };

        float m_FrameTimer{ 0.f };
        float m_LifeTimer { 0.f };
        int m_Frame { 0 };

        bool  m_IsConfigured{ false };

        void FinishFire();
    };
}

#endif
