#include <Components/LivesDisplayComponent.h>
#include <Components/RenderComponent.h>
#include <ComponentFactoryRegistry.h>
#include <ComponentTypeMap.h>
#include <ResourceManager.h>
#include <GameObject.h>
#include <Renderer.h>
#include <GameManager.h>

namespace
{
	const bool SpinnerComponentRegistered = dae::RegisterComponentFactoryFor<dae::LivesDisplayComponent>(dae::HASH_LivesDisplayComponent);
}


namespace dae
{
	void LivesDisplayComponent::Init()
	{
		m_pTexture = ResourceManager::GetInstance().LoadTexture(m_LivesTextureName);
	}

	void LivesDisplayComponent::Update()
	{

	}
	void LivesDisplayComponent::LateUpdate()
	{

	}

	void LivesDisplayComponent::Render() const
	{
		if (m_pTexture == nullptr)
		{
			return;
		}
		auto& renderer = Renderer::GetInstance();

		auto pos = this->GetParent()->GetWorldPosition();
		const auto inc = m_pTexture.get()->GetSize().y + 2;
		const auto lc = GameManager::GetInstance().GetGameData().m_Lives - 1;
		for (int i = 0; i < lc; ++i)
		{
			renderer.RenderTexture(*m_pTexture, pos.x, pos.y + (inc * i));
		}
	}

	bool LivesDisplayComponent::Deserialize(const std::map<std::string, std::string>& properties, std::string& errorMessage)
	{
		if (!GetRequiredProperty(properties, "LivesTextureName", m_LivesTextureName, errorMessage, "LivesDisplayComponent")) return false;

		return true;
	}
};