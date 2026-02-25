#include "SpinnerComponent.h"
#include "Timing.h"
#include "GameObject.h"

dae::SpinnerComonent::SpinnerComonent(GameObject* Parent, bool SpinClockwise, float SpinRadius, float SpinSpeed, glm::vec2 SpinPoint) : Component(Parent),
m_spinClockwise(SpinClockwise), m_spinRadius(SpinRadius), m_spinSpeed(SpinSpeed), m_spinPoint(SpinPoint)
{

}

void dae::SpinnerComonent::Update()
{
	float DT = Timing::GetInstance().GetDeltaTime();
	float direction = m_spinClockwise ? -1.0f : 1.0f;
	m_spinAngle += direction * m_spinSpeed * DT;

	glm::vec2 LocalPos = GetParent()->GetLocalPosition();
	LocalPos.x = m_spinPoint.x + cos(m_spinAngle) * m_spinRadius;
	LocalPos.y = m_spinPoint.y + sin(m_spinAngle) * m_spinRadius;
	GetParent()->SetLocalPosition(LocalPos);
}

void dae::SpinnerComonent::LateUpdate()
{
}

void dae::SpinnerComonent::Init()
{
}
