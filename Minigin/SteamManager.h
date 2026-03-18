#pragma once

#ifdef USE_STEAMWORKS
#include "Singleton.h"
#include <steam_api.h>

namespace dae
{
	class SteamManager final : public Singleton<SteamManager>
	{
	public:
		SteamManager();
		~SteamManager();

		void ProcessEvents();
		void Init();
		void UnlockAchievement(const char* pchAchievementName);

		SteamManager(const SteamManager& other) = delete;
		SteamManager(SteamManager&& other) = delete;
		SteamManager& operator=(const SteamManager& other) = delete;
		SteamManager& operator=(SteamManager&& other) = delete;
	};
}
#endif
