#ifndef _SCORECOMPONENT_H_
#define _SCORECOMPONENT_H_

#include <Component.h>
#include <Hash.h>
#include <EventManager.h>
#include <cstdint>

namespace dae
{
	class GameObject;
	class TextRenderComponent;
	class ScoreComponent : public Component
	{
	public:
		ScoreComponent(GameObject* Parent) : Component(Parent) {};
		~ScoreComponent();

		void Init() override;
		void Update() override;
		void LateUpdate() override;

		static constexpr unsigned int SCOREHASH = make_sdbm_hash("IncreaseScore");

	private:
		bool Deserialize(const std::map<std::string, std::string>& properties, std::string& errorMessage) override;

		EventManager::EventId m_IncreaseScoreEventID{ 0 };
		TextRenderComponent* m_pTRC{ nullptr };
		uint32_t m_Score{ 0 };
	};
};

#endif