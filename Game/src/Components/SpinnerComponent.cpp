#include "SpinnerComponent.h"
#include "Timing.h"
#include "GameObject.h"
#include "Debugger.h"
#include <cmath>
#include <sstream>
#include <format>
#include <map>

dae::SpinnerComponent::SpinnerComponent(GameObject* Parent, bool SpinClockwise, float SpinRadius, float SpinSpeed, glm::vec2 SpinPoint) : Component(Parent),
m_spinClockwise(SpinClockwise), m_spinRadius(SpinRadius), m_spinSpeed(SpinSpeed), m_spinPoint(SpinPoint)
{

}

void dae::SpinnerComponent::Update()
{
	float DT = Timing::GetInstance().GetDeltaTime();
	float direction = m_spinClockwise ? -1.0f : 1.0f;
	m_spinAngle += direction * m_spinSpeed * DT;

	glm::vec2 LocalPos = GetParent()->GetLocalPosition();
	LocalPos.x = m_spinPoint.x + cos(m_spinAngle) * m_spinRadius;
	LocalPos.y = m_spinPoint.y + sin(m_spinAngle) * m_spinRadius;
	GetParent()->SetLocalPosition(LocalPos);
}

void dae::SpinnerComponent::LateUpdate()
{
}

void dae::SpinnerComponent::Init()
{
}

bool dae::SpinnerComponent::Deserialize(const std::map<std::string, std::string>& properties, std::string& errorMessage)
{
	auto clockwiseIt = properties.find("spinClockwise");
	auto radiusIt = properties.find("spinRadius");
	auto speedIt = properties.find("spinSpeed");
	auto pointIt = properties.find("spinPoint");

	if (clockwiseIt != properties.end() && radiusIt != properties.end() &&
		speedIt != properties.end() && pointIt != properties.end())
	{
		try
		{
			if (!clockwiseIt->second.empty())
				m_spinClockwise = (clockwiseIt->second == "true" || clockwiseIt->second == "1");
			else
			{
				m_spinClockwise = true;
				Debugger::GetInstance().LogDebug("spinClockwise empty, using default (true)");
			}

			if (!radiusIt->second.empty())
				m_spinRadius = std::stof(radiusIt->second);
			else
			{
				m_spinRadius = 50.0f;
				Debugger::GetInstance().LogDebug("spinRadius empty, using default (50.0f)");
			}

			if (!speedIt->second.empty())
				m_spinSpeed = std::stof(speedIt->second);
			else
			{
				m_spinSpeed = 1.0f;
				Debugger::GetInstance().LogDebug("spinSpeed empty, using default (1.0f)");
			}

			if (!pointIt->second.empty())
			{
				std::istringstream pointStream(pointIt->second);
				std::string token;
				float values[2] = {400.0f, 300.0f};
				int valueCount = 0;

				while (valueCount < 2 && std::getline(pointStream, token, ','))
				{
					try
					{
						values[valueCount] = std::stof(token);
						valueCount++;
					}
					catch (const std::exception&)
					{
						break;
					}
				}

				if (valueCount == 2)
				{
					m_spinPoint = glm::vec2(values[0], values[1]);
				}
				else
				{
					m_spinPoint = glm::vec2(400.0f, 300.0f);
					Debugger::GetInstance().LogWarning(std::format(
						"Failed to parse spinPoint: {}, using default", pointIt->second
					));
				}
			}
			else
			{
				m_spinPoint = glm::vec2(400.0f, 300.0f);
				Debugger::GetInstance().LogDebug("spinPoint empty, using default (400,300)");
			}

			return true;
		}
		catch (const std::exception& e)
		{
			errorMessage = std::format("Failed to deserialize SpinnerComponent: {}", e.what());
			Debugger::GetInstance().LogError(errorMessage);
			return false;
		}
	}
	else
	{
		errorMessage = "SpinnerComponent missing required properties";
		Debugger::GetInstance().LogError(errorMessage);
		return false;
	}
}