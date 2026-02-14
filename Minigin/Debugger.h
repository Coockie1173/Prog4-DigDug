#ifndef _DEBUGGER_ENGINE_
#define _DEBUGGER_ENGINE_

#include "Singleton.h"
#include <string>
#include <SDL3_ttf/SDL_ttf.h>
#include <memory>

namespace dae
{
	class Scene;
	class Font;
};

//simple debug logger since console is disabled and I want something pretty to log to
class Debugger : public dae::Singleton<Debugger>
{
public:
	void LogDebug(const std::string& TextToLog, bool Logtime = true);
	void LogWarning(const std::string& TextToLog, bool Logtime = true);
	void LogError(const std::string& TextToLog, bool Logtime = true);
	void LogMessage(const std::string& Text, const SDL_Color& LogColour, bool Logtime = true);
	void AttachToScene(dae::Scene* Scene) { m_attachedScene = Scene; };
	void DeleteAllLogs();
private:
	int m_messageCount{0};
	std::shared_ptr<dae::Font> m_debugFont{nullptr};
	dae::Scene* m_attachedScene{nullptr};
};

#endif // !_DEBUGGER_ENGINE_
