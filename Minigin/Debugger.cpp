#include "Debugger.h"
#include "Font.h"
#include <SDL3_ttf/SDL_ttf.h>
#include <ResourceManager.h>
#include <GameObject.h>
#include <Components/TextRendererComponent.h>
#include <Scene.h>
#include <string>
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include "Texture2D.h"

constexpr int DebugTextSize = 24;
constexpr SDL_Color DebugColor{ 111, 227, 235, 255 };
constexpr SDL_Color WarningColor{ 255, 207, 20, 255 };
constexpr SDL_Color ErrorColor{ 255, 0, 0, 255 };


void Debugger::LogDebug(const std::string& TextToLog, bool Logtime)
{
	LogMessage("INFO: " + TextToLog, DebugColor, Logtime);
}

void Debugger::LogWarning(const std::string& TextToLog, bool Logtime)
{
	LogMessage("WARNING: " + TextToLog, WarningColor, Logtime);
}

void Debugger::LogError(const std::string& TextToLog, bool Logtime)
{
	LogMessage("ERROR: " + TextToLog, ErrorColor, Logtime);
}

void Debugger::LogMessage(const std::string& Text, const SDL_Color& LogColour, bool Logtime)
{
	if (m_debugFont == nullptr)
	{
		m_debugFont = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", DebugTextSize);
	}
	
	std::string PrintText{};
	if (Logtime)
	{
		auto now = std::chrono::system_clock::now();
		std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
		std::tm local_tm;
		localtime_s(&local_tm, &now_time_t); 

		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

		std::ostringstream oss;
		oss << std::put_time(&local_tm, "%H:%M:%S")
			<< '.' << std::setw(3) << std::setfill('0') << ms.count();

		std::string time_str = oss.str();
		PrintText += time_str;
		PrintText += " - ";
	}

	PrintText += Text;
	const std::string ObjectName = "Debug_Message_" + std::to_string(m_messageCount);
	

	auto to = std::make_unique<dae::GameObject>(ObjectName, true);
	to->AddComponent<TextRenderComponent>(PrintText, LogColour, m_debugFont);
	to->SetPosition(10, (float)(((DebugTextSize + 1) * m_messageCount) + 10));
	m_messageCount++;
	m_attachedScene->Add(std::move(to));
}

void Debugger::DeleteAllLogs()
{
	m_messageCount = 0;
	m_attachedScene->CleanupDebug();
}
