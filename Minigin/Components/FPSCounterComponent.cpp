#include "FPSCounterComponent.h"
#include "GameObject.h"
#include "RenderComponent.h"
#include "TextRendererComponent.h"
#include "Debugger.h"
#include "Timing.h"
#include <string>

namespace dae
{
	FPSCounterComponent::FPSCounterComponent(GameObject* Parent) : Component(Parent)
	{
		m_parentRenderer = nullptr;
	}

	void FPSCounterComponent::Update()
	{
		float DT = Timing::GetInstance().GetDeltaTime();

		if (m_parentRenderer == nullptr)
		{
			return;
		}
		if (m_UpdateTimer > 0)
		{
			m_UpdateTimer -= DT;
			return;
		}
		m_UpdateTimer = 1;
		float FPS = 1 / DT;
		std::string FPSText = "FPS: ";
		FPSText += std::to_string(FPS);
		m_parentRenderer->SetText(FPSText);
	}

	void FPSCounterComponent::LateUpdate()
	{
	}

	void FPSCounterComponent::Init()
	{
		auto rend = GetParent()->GetAttachedRenderer();
		if (rend == nullptr)
		{
			Debugger::GetInstance().LogError("Can't use FPSCounter without a renderer!");
			return;
		}

		if(auto r = dynamic_cast<TextRenderComponent*>(rend))
		{
			if (!r)
			{
				Debugger::GetInstance().LogError("Can't use FPSCounter on a non-text renderer!");
				return;
			}
			m_parentRenderer = dynamic_cast<TextRenderComponent*>(rend);
		}

		m_UpdateTimer = 1;
	}
};
