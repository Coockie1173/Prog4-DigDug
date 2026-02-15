#include "FPSCounterComponent.h"
#include "GameObject.h"
#include "RenderComponent.h"
#include "TextRendererComponent.h"
#include "Debugger.h"
#include "Timing.h"
#include <string>

FPSCounterComponent::FPSCounterComponent(dae::GameObject* Parent) : Component(Parent)
{
	m_parentRenderer = nullptr;
}

void FPSCounterComponent::Update()
{
	if (m_parentRenderer == nullptr)
	{
		return;
	}
	float FPS = 1 / Timing::GetInstance().GetDeltaTime();
	std::string FPSText = "FPS: ";
	FPSText += std::to_string(FPS);
	m_parentRenderer->SetText(FPSText);
}

void FPSCounterComponent::LateUpdate()
{
}

void FPSCounterComponent::Init()
{
	auto rend = m_parent->GetAttachedRenderer();
	if (rend == nullptr)
	{
		Debugger::GetInstance().LogError("Can't use FPSCounter without a renderer!");
		return;
	}

	if (rend->GetSubtype() != RenderSubtype::RENDER_TEXT)
	{
		Debugger::GetInstance().LogError("Can't use FPSCounter on a non-text renderer!");
		return;
	}

	m_parentRenderer = dynamic_cast<TextRenderComponent*>(rend);
}
