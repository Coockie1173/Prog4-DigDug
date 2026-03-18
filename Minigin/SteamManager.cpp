#include "SteamManager.h"

#ifdef USE_STEAMWORKS
#include <iostream>
#endif

dae::SteamManager::SteamManager()
{

}

dae::SteamManager::~SteamManager()
{
#ifdef USE_STEAMWORKS
	SteamAPI_Shutdown();
#endif
}

void dae::SteamManager::ProcessEvents()
{
#ifdef USE_STEAMWORKS
	SteamAPI_RunCallbacks();
#endif
}

void dae::SteamManager::Init()
{
#ifdef USE_STEAMWORKS
	if (!SteamAPI_Init())
	{
		std::cerr << "SteamAPI_Init() failed\n";
		return;
	}
#endif
}

void dae::SteamManager::UnlockAchievement(const char* pchAchievementName)
{
#ifdef USE_STEAMWORKS
	if (SteamUserStats() != nullptr)
	{
		SteamUserStats()->SetAchievement(pchAchievementName);
		SteamUserStats()->StoreStats();
	}
#endif
}
