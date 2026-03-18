#pragma once
#include "Singleton.h"

#ifdef USE_STEAMWORKS
#include <steam_api.h>
#endif

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
