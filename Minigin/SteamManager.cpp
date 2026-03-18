#include "SteamManager.h"

#ifdef USE_STEAMWORKS
#include <iostream>

dae::SteamManager::SteamManager()
{

}

dae::SteamManager::~SteamManager()
{
	SteamAPI_Shutdown();
}

void dae::SteamManager::ProcessEvents()
{
	SteamAPI_RunCallbacks();
}

void dae::SteamManager::Init()
{
	if (!SteamAPI_Init())
	{
		std::cerr << "SteamAPI_Init() failed\n";
		return;
	}
}

void dae::SteamManager::UnlockAchievement(const char* pchAchievementName)
{
	if (SteamUserStats() != nullptr)
	{
		SteamUserStats()->SetAchievement(pchAchievementName);
		SteamUserStats()->StoreStats();
	}
}

#endif
