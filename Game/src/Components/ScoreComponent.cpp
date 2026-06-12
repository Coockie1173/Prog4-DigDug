#include <Components/ScoreComponent.h>
#include <ComponentFactoryRegistry.h>
#include <ComponentTypeMap.h>
#include <TextRendererComponent.h>
#include <GameObject.h>
#include <GameManager.h>

namespace
{
	const bool SpinnerComponentRegistered = dae::RegisterComponentFactoryFor<dae::ScoreComponent>(dae::HASH_ScoreComponent);
}

namespace dae
{
	void ScoreComponent::Init()
	{
		m_pTRC = GetParent()->GetComponent<TextRenderComponent>();
		m_pTRC->SetText(std::to_string(GameManager::GetInstance().GetGameData().m_Score));

		m_IncreaseScoreEventID = EventManager::GetInstance().Subscribe(ScoreComponent::SCOREHASH, 
			[this](unsigned int, const std::any& Data)
			{
				this->m_Score += std::any_cast<uint32_t>(Data);
				m_pTRC->SetText(std::to_string(m_Score));
			});
	}

	void ScoreComponent::Update()
	{

	}

	void ScoreComponent::LateUpdate()
	{

	}

	bool ScoreComponent::Deserialize(const std::map<std::string, std::string>&, std::string&)
	{
		return true;
	}

	ScoreComponent::~ScoreComponent()
	{
		EventManager::GetInstance().Unsubscribe(ScoreComponent::SCOREHASH, m_IncreaseScoreEventID);
	}
};